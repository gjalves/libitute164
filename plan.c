#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libitute164_private.h"

struct plan_area {
    int area_code;
    enum itu_t_area_type_enum type;
    struct plan_area *next;
};

struct plan_area_country {
    int area_code;
    char *country;
    struct plan_area_country *next;
};

struct plan_data {
    unsigned char cc_set[1000];
    enum itu_t_e164_type_enum cc[1000];
    char *countries[1000];
    char *national_prefixes[1000];
    char *international_prefixes[1000];
    struct plan_area *areas[1000];
    struct plan_area_country *area_countries[1000];
    struct cc_regex *subscribers[1000];
    size_t subscriber_count[1000];
};

static struct plan_data active_plan;
static char plan_error[256];

static int set_plan_error(unsigned long line_no, const char *fmt, ...)
{
    int pos;
    va_list ap;

    if(line_no > 0)
        pos = snprintf(plan_error, sizeof(plan_error), "line %lu: ", line_no);
    else
        pos = snprintf(plan_error, sizeof(plan_error), "plan: ");

    if(pos < 0 || pos >= (int)sizeof(plan_error))
        return -1;

    va_start(ap, fmt);
    vsnprintf(plan_error + pos, sizeof(plan_error) - pos, fmt, ap);
    va_end(ap);
    return -1;
}

const char *itu_t_e164_plan_error(void)
{
    return plan_error[0] == 0 ? NULL : plan_error;
}

static char *plan_strdup(const char *value)
{
    char *copy;
    size_t len;

    len = strlen(value) + 1;
    copy = malloc(len);
    if(copy == NULL) return NULL;
    memcpy(copy, value, len);
    return copy;
}

static int parse_int_range(const char *value, int min, int max, int *out)
{
    char *end;
    long parsed;

    errno = 0;
    parsed = strtol(value, &end, 10);
    if(errno != 0 || *end != 0 || parsed < min || parsed > max)
        return -1;

    *out = (int)parsed;
    return 0;
}

static int parse_cc_type(const char *value, enum itu_t_e164_type_enum *type)
{
    if(strcmp(value, "unknown") == 0) *type = ITU_T_UNKNOWN;
    else if(strcmp(value, "incomplete") == 0) *type = ITU_T_INCOMPLETE;
    else if(strcmp(value, "reserved") == 0) *type = ITU_T_RESERVED;
    else if(strcmp(value, "spare") == 0) *type = ITU_T_SPARE;
    else if(strcmp(value, "number") == 0) *type = ITU_T_NUMBER;
    else if(strcmp(value, "geographic") == 0) *type = ITU_T_GEOGRAPHIC;
    else if(strcmp(value, "global") == 0) *type = ITU_T_GLOBAL;
    else if(strcmp(value, "groups") == 0) *type = ITU_T_GROUPS;
    else if(strcmp(value, "trials") == 0) *type = ITU_T_TRIALS;
    else return -1;

    return 0;
}

static int parse_area_type(const char *value, enum itu_t_area_type_enum *type)
{
    if(strcmp(value, "unknown") == 0) *type = ITU_T_AREA_UNKNOWN;
    else if(strcmp(value, "incomplete") == 0) *type = ITU_T_AREA_INCOMPLETE;
    else if(strcmp(value, "number") == 0) *type = ITU_T_AREA_NUMBER;
    else return -1;

    return 0;
}

static int parse_number_kind(const char *value, enum itu_t_e164_number_kind_enum *kind)
{
    if(strcmp(value, "unknown") == 0) *kind = ITU_T_E164_NUMBER_KIND_UNKNOWN;
    else if(strcmp(value, "regular") == 0) *kind = ITU_T_E164_NUMBER_KIND_REGULAR;
    else if(strcmp(value, "toll-free") == 0) *kind = ITU_T_E164_NUMBER_KIND_TOLL_FREE;
    else if(strcmp(value, "short") == 0) *kind = ITU_T_E164_NUMBER_KIND_SHORT;
    else if(strcmp(value, "premium") == 0) *kind = ITU_T_E164_NUMBER_KIND_PREMIUM;
    else if(strcmp(value, "emergency") == 0) *kind = ITU_T_E164_NUMBER_KIND_EMERGENCY;
    else if(strcmp(value, "service") == 0) *kind = ITU_T_E164_NUMBER_KIND_SERVICE;
    else return -1;

    return 0;
}

static int validate_regex(const char *value)
{
    regex_t regex;
    int ret;

    ret = regcomp(&regex, value, REG_EXTENDED);
    if(ret == 0) regfree(&regex);
    return ret == 0 ? 0 : -1;
}

static int validate_mask(const char *value)
{
    return strchr(value, '#') == NULL ? -1 : 0;
}

static int validate_country_tag(const char *value)
{
    return strlen(value) == 5 &&
           islower((unsigned char)value[0]) &&
           islower((unsigned char)value[1]) &&
           value[2] == '_' &&
           isupper((unsigned char)value[3]) &&
           isupper((unsigned char)value[4]) ? 0 : -1;
}

static int validate_dial_prefix(const char *value)
{
    size_t i;

    if(value[0] == 0)
        return -1;

    for(i = 0; value[i] != 0; i++) {
        if(!isdigit((unsigned char)value[i]))
            return -1;
    }

    return 0;
}

static void free_regex_entry(struct cc_regex *entry)
{
    free((char *)entry->regex_ndc);
    free((char *)entry->regex_sn);
    free((char *)entry->mask_sn);
}

static void free_plan_data(struct plan_data *plan)
{
    int i;

    for(i = 0; i < 1000; i++) {
        struct plan_area *area = plan->areas[i];

        while(area != NULL) {
            struct plan_area *next = area->next;
            free(area);
            area = next;
        }
        plan->areas[i] = NULL;

        {
            struct plan_area_country *area_country = plan->area_countries[i];

            while(area_country != NULL) {
                struct plan_area_country *next = area_country->next;
                free(area_country->country);
                free(area_country);
                area_country = next;
            }
            plan->area_countries[i] = NULL;
        }

        if(plan->subscribers[i] != NULL) {
            size_t j;
            for(j = 0; j < plan->subscriber_count[i]; j++)
                free_regex_entry(&plan->subscribers[i][j]);
            free(plan->subscribers[i]);
        }
        plan->subscribers[i] = NULL;
        plan->subscriber_count[i] = 0;

        free(plan->countries[i]);
        plan->countries[i] = NULL;

        free(plan->national_prefixes[i]);
        plan->national_prefixes[i] = NULL;

        free(plan->international_prefixes[i]);
        plan->international_prefixes[i] = NULL;
    }

    memset(plan->cc_set, 0, sizeof(plan->cc_set));
    memset(plan->cc, 0, sizeof(plan->cc));
}

void itu_t_e164_reset_plan(void)
{
    free_plan_data(&active_plan);
}

static void activate_plan(struct plan_data *plan)
{
    struct plan_data old_plan = active_plan;

    active_plan = *plan;
    memset(plan, 0, sizeof(*plan));
    free_plan_data(&old_plan);
}

static int add_cc(struct plan_data *plan, int country_code, enum itu_t_e164_type_enum type)
{
    plan->cc[country_code] = type;
    plan->cc_set[country_code] = 1;
    return 0;
}

static int add_area(struct plan_data *plan, int country_code, int area_code, enum itu_t_area_type_enum type)
{
    struct plan_area *area;

    for(area = plan->areas[country_code]; area != NULL; area = area->next) {
        if(area->area_code == area_code) {
            area->type = type;
            return 0;
        }
    }

    area = malloc(sizeof(*area));
    if(area == NULL) return -1;

    area->area_code = area_code;
    area->type = type;
    area->next = plan->areas[country_code];
    plan->areas[country_code] = area;
    return 0;
}

static int add_country(struct plan_data *plan, int country_code, const char *country)
{
    char *copy;

    if(validate_country_tag(country) != 0)
        return -1;

    copy = plan_strdup(country);
    if(copy == NULL) return -1;

    free(plan->countries[country_code]);
    plan->countries[country_code] = copy;
    return 0;
}

static int add_area_country(struct plan_data *plan, int country_code, int area_code, const char *country)
{
    struct plan_area_country *area_country;
    char *copy;

    if(validate_country_tag(country) != 0)
        return -1;

    copy = plan_strdup(country);
    if(copy == NULL) return -1;

    for(area_country = plan->area_countries[country_code]; area_country != NULL; area_country = area_country->next) {
        if(area_country->area_code == area_code) {
            free(area_country->country);
            area_country->country = copy;
            return 0;
        }
    }

    area_country = malloc(sizeof(*area_country));
    if(area_country == NULL) {
        free(copy);
        return -1;
    }

    area_country->area_code = area_code;
    area_country->country = copy;
    area_country->next = plan->area_countries[country_code];
    plan->area_countries[country_code] = area_country;
    return 0;
}

static int add_prefix(char **prefixes, int country_code, const char *prefix)
{
    char *copy;

    if(validate_dial_prefix(prefix) != 0)
        return -1;

    copy = plan_strdup(prefix);
    if(copy == NULL) return -1;

    free(prefixes[country_code]);
    prefixes[country_code] = copy;
    return 0;
}

static int add_subscriber(struct plan_data *plan, int country_code, const char *regex_ndc, const char *regex_sn, const char *mask, enum itu_t_e164_number_kind_enum kind)
{
    struct cc_regex *entries;
    struct cc_regex *entry;
    size_t count;

    if(strcmp(regex_ndc, "*") != 0 && validate_regex(regex_ndc) != 0)
        return -1;
    if(validate_regex(regex_sn) != 0 || validate_mask(mask) != 0)
        return -1;

    count = plan->subscriber_count[country_code];
    entries = realloc(plan->subscribers[country_code], (count + 2) * sizeof(*entries));
    if(entries == NULL) return -1;
    plan->subscribers[country_code] = entries;

    entry = &entries[count];
    entry->regex_ndc = strcmp(regex_ndc, "*") == 0 ? NULL : plan_strdup(regex_ndc);
    entry->regex_sn = plan_strdup(regex_sn);
    entry->mask_sn = plan_strdup(mask);
    entry->kind = kind;

    if((strcmp(regex_ndc, "*") != 0 && entry->regex_ndc == NULL) || entry->regex_sn == NULL || entry->mask_sn == NULL) {
        free_regex_entry(entry);
        return -1;
    }

    entries[count + 1].regex_ndc = NULL;
    entries[count + 1].regex_sn = NULL;
    entries[count + 1].mask_sn = NULL;
    entries[count + 1].kind = ITU_T_E164_NUMBER_KIND_UNKNOWN;
    plan->subscriber_count[country_code] = count + 1;
    return 0;
}

static int tokenize_line(char *line, char **argv, int max_args)
{
    int argc = 0;
    char *p = line;

    while(*p != 0) {
        while(isspace((unsigned char)*p)) p++;
        if(*p == 0 || (argc == 0 && *p == '#')) break;
        if(argc == max_args) return -1;

        if(*p == '"') {
            argv[argc++] = ++p;
            while(*p != 0 && *p != '"') p++;
            if(*p != '"') return -1;
            *p++ = 0;
        } else {
            argv[argc++] = p;
            while(*p != 0 && !isspace((unsigned char)*p)) p++;
            if(*p != 0) *p++ = 0;
        }
    }

    return argc;
}

static int parse_line(struct plan_data *plan, char *line, unsigned long line_no)
{
    char *argv[6];
    int argc;
    int country_code;

    argc = tokenize_line(line, argv, 6);
    if(argc < 0) return set_plan_error(line_no, "invalid quoting or too many fields");
    if(argc == 0) return 0;

    if(strcmp(argv[0], "cc") == 0) {
        enum itu_t_e164_type_enum type;
        if(argc != 3) return set_plan_error(line_no, "cc requires 2 arguments");
        if(parse_int_range(argv[1], 0, 999, &country_code) != 0)
            return set_plan_error(line_no, "invalid country code '%s'", argv[1]);
        if(parse_cc_type(argv[2], &type) != 0)
            return set_plan_error(line_no, "invalid country code type '%s'", argv[2]);
        if(add_cc(plan, country_code, type) != 0)
            return set_plan_error(line_no, "could not store country code rule");
        return 0;
    }

    if(strcmp(argv[0], "area") == 0) {
        enum itu_t_area_type_enum type;
        int area_code;

        if(argc != 4) return set_plan_error(line_no, "area requires 3 arguments");
        if(parse_int_range(argv[1], 0, 999, &country_code) != 0)
            return set_plan_error(line_no, "invalid country code '%s'", argv[1]);
        if(parse_int_range(argv[2], 0, 9999, &area_code) != 0)
            return set_plan_error(line_no, "invalid area code '%s'", argv[2]);
        if(parse_area_type(argv[3], &type) != 0)
            return set_plan_error(line_no, "invalid area type '%s'", argv[3]);
        if(add_area(plan, country_code, area_code, type) != 0)
            return set_plan_error(line_no, "could not store area rule");
        return 0;
    }

    if(strcmp(argv[0], "country") == 0) {
        if(argc != 3) return set_plan_error(line_no, "country requires 2 arguments");
        if(parse_int_range(argv[1], 0, 999, &country_code) != 0)
            return set_plan_error(line_no, "invalid country code '%s'", argv[1]);
        if(add_country(plan, country_code, argv[2]) != 0)
            return set_plan_error(line_no, "invalid country tag '%s'", argv[2]);
        return 0;
    }

    if(strcmp(argv[0], "area-country") == 0) {
        int area_code;

        if(argc != 4) return set_plan_error(line_no, "area-country requires 3 arguments");
        if(parse_int_range(argv[1], 0, 999, &country_code) != 0)
            return set_plan_error(line_no, "invalid country code '%s'", argv[1]);
        if(parse_int_range(argv[2], 0, 9999, &area_code) != 0)
            return set_plan_error(line_no, "invalid area code '%s'", argv[2]);
        if(add_area_country(plan, country_code, area_code, argv[3]) != 0)
            return set_plan_error(line_no, "invalid country tag '%s'", argv[3]);
        return 0;
    }

    if(strcmp(argv[0], "national-prefix") == 0) {
        if(argc != 3) return set_plan_error(line_no, "national-prefix requires 2 arguments");
        if(parse_int_range(argv[1], 0, 999, &country_code) != 0)
            return set_plan_error(line_no, "invalid country code '%s'", argv[1]);
        if(add_prefix(plan->national_prefixes, country_code, argv[2]) != 0)
            return set_plan_error(line_no, "invalid national prefix '%s'", argv[2]);
        return 0;
    }

    if(strcmp(argv[0], "international-prefix") == 0) {
        if(argc != 3) return set_plan_error(line_no, "international-prefix requires 2 arguments");
        if(parse_int_range(argv[1], 0, 999, &country_code) != 0)
            return set_plan_error(line_no, "invalid country code '%s'", argv[1]);
        if(add_prefix(plan->international_prefixes, country_code, argv[2]) != 0)
            return set_plan_error(line_no, "invalid international prefix '%s'", argv[2]);
        return 0;
    }

    if(strcmp(argv[0], "subscriber") == 0) {
        enum itu_t_e164_number_kind_enum kind = ITU_T_E164_NUMBER_KIND_REGULAR;

        if(argc != 5 && argc != 6) return set_plan_error(line_no, "subscriber requires 4 or 5 arguments");
        if(parse_int_range(argv[1], 0, 999, &country_code) != 0)
            return set_plan_error(line_no, "invalid country code '%s'", argv[1]);
        if(argc == 6 && parse_number_kind(argv[5], &kind) != 0)
            return set_plan_error(line_no, "invalid number kind '%s'", argv[5]);
        if(add_subscriber(plan, country_code, argv[2], argv[3], argv[4], kind) != 0)
            return set_plan_error(line_no, "invalid subscriber regex or mask");
        return 0;
    }

    return set_plan_error(line_no, "unknown directive '%s'", argv[0]);
}

int itu_t_e164_load_plan_fp(FILE *fp)
{
    char line[1024];
    unsigned long line_no = 0;
    struct plan_data plan;

    plan_error[0] = 0;
    if(fp == NULL) return set_plan_error(0, "null file pointer");

    memset(&plan, 0, sizeof(plan));
    while(fgets(line, sizeof(line), fp) != NULL) {
        line_no++;
        if(parse_line(&plan, line, line_no) != 0) {
            free_plan_data(&plan);
            return -1;
        }
    }

    if(ferror(fp)) {
        free_plan_data(&plan);
        return set_plan_error(0, "read error");
    }

    activate_plan(&plan);
    plan_error[0] = 0;
    return 0;
}

int itu_t_e164_load_plan_memory(const char *data, size_t size)
{
    char line[1024];
    unsigned long line_no = 0;
    struct plan_data plan;
    size_t pos = 0;

    plan_error[0] = 0;
    if(data == NULL && size > 0)
        return set_plan_error(0, "null memory pointer");

    memset(&plan, 0, sizeof(plan));
    while(pos < size) {
        size_t line_len = 0;

        line_no++;
        while(pos < size && data[pos] != '\n') {
            if(line_len >= sizeof(line) - 1) {
                free_plan_data(&plan);
                return set_plan_error(line_no, "line too long");
            }
            line[line_len++] = data[pos++];
        }

        if(pos < size && data[pos] == '\n')
            pos++;

        line[line_len] = 0;
        if(parse_line(&plan, line, line_no) != 0) {
            free_plan_data(&plan);
            return -1;
        }
    }

    activate_plan(&plan);
    plan_error[0] = 0;
    return 0;
}

int itu_t_e164_load_plan_file(const char *path)
{
    FILE *fp;
    int ret;

    plan_error[0] = 0;
    if(path == NULL) return set_plan_error(0, "null path");

    fp = fopen(path, "r");
    if(fp == NULL) return set_plan_error(0, "could not open '%s'", path);

    ret = itu_t_e164_load_plan_fp(fp);
    fclose(fp);
    return ret;
}

int itu_t_e164_load_default_plan(void)
{
    const char *env_path;
    static const char *paths[] = {
        "/etc/libitute164/e164-plan.txt",
        "/usr/share/libitute164/e164-plan.txt",
    };
    size_t i;

    env_path = getenv("LIBITUTE164_PLAN");
    if(env_path != NULL && env_path[0] != 0)
        return itu_t_e164_load_plan_file(env_path);

    plan_error[0] = 0;
    for(i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        FILE *fp = fopen(paths[i], "r");
        int ret;

        if(fp == NULL)
            continue;

        ret = itu_t_e164_load_plan_fp(fp);
        fclose(fp);
        if(ret == 0)
            return 0;
        return -1;
    }

    return set_plan_error(0, "default numbering plan not found");
}

int itu_t_e164_plan_cc_lookup(int country_code, enum itu_t_e164_type_enum *type)
{
    if(country_code < 0 || country_code >= 1000 || !active_plan.cc_set[country_code])
        return 0;

    *type = active_plan.cc[country_code];
    return 1;
}

const char *itu_t_e164_cc_2_country(int country_code)
{
    if(country_code < 0 || country_code >= 1000)
        return NULL;

    return active_plan.countries[country_code];
}

const char *itu_t_e164_plan_area_country_lookup(int country_code, int area_code)
{
    struct plan_area_country *area_country;

    if(country_code < 0 || country_code >= 1000)
        return NULL;

    for(area_country = active_plan.area_countries[country_code]; area_country != NULL; area_country = area_country->next) {
        if(area_country->area_code == area_code)
            return area_country->country;
    }

    return NULL;
}

const char *itu_t_e164_area_2_country(int country_code, int area_code)
{
    const char *country;

    country = itu_t_e164_plan_area_country_lookup(country_code, area_code);
    return country != NULL ? country : itu_t_e164_cc_2_country(country_code);
}

const char *itu_t_e164_get_country(const itu_t_e164_t *e164)
{
    if(e164 == NULL || e164->cc.type == ITU_T_UNKNOWN || e164->cc.type == ITU_T_INCOMPLETE)
        return NULL;

    if(e164->cc.type == ITU_T_NUMBER && e164->number.ndc_len > 0)
        return itu_t_e164_area_2_country(e164->cc.value, e164->number.ndc);

    return itu_t_e164_cc_2_country(e164->cc.value);
}

enum itu_t_e164_number_kind_enum itu_t_e164_get_number_kind(const itu_t_e164_t *e164)
{
    if(e164 == NULL || e164->cc.type != ITU_T_NUMBER)
        return ITU_T_E164_NUMBER_KIND_UNKNOWN;

    return e164->number.kind;
}

const char *itu_t_e164_number_kind_name(enum itu_t_e164_number_kind_enum kind)
{
    switch(kind) {
        case ITU_T_E164_NUMBER_KIND_REGULAR:
            return "regular";
        case ITU_T_E164_NUMBER_KIND_TOLL_FREE:
            return "toll-free";
        case ITU_T_E164_NUMBER_KIND_SHORT:
            return "short";
        case ITU_T_E164_NUMBER_KIND_PREMIUM:
            return "premium";
        case ITU_T_E164_NUMBER_KIND_EMERGENCY:
            return "emergency";
        case ITU_T_E164_NUMBER_KIND_SERVICE:
            return "service";
        default:
            return "unknown";
    }
}

const char *itu_t_e164_cc_2_national_prefix(int country_code)
{
    if(country_code < 0 || country_code >= 1000)
        return NULL;

    return active_plan.national_prefixes[country_code];
}

const char *itu_t_e164_cc_2_international_prefix(int country_code)
{
    if(country_code < 0 || country_code >= 1000)
        return NULL;

    return active_plan.international_prefixes[country_code];
}

int itu_t_e164_plan_area_lookup(int country_code, int area_code, enum itu_t_area_type_enum *type)
{
    struct plan_area *area;

    if(country_code < 0 || country_code >= 1000)
        return 0;

    for(area = active_plan.areas[country_code]; area != NULL; area = area->next) {
        if(area->area_code == area_code) {
            *type = area->type;
            return 1;
        }
    }

    return 0;
}

struct cc_regex *itu_t_e164_plan_subscriber_regex(int country_code)
{
    if(country_code < 0 || country_code >= 1000)
        return NULL;

    return active_plan.subscribers[country_code];
}
