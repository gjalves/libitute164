#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <regex.h>
#include <err.h>
#include "libitute164.h"

// Initialize structure
void itu_t_e164_init(itu_t_e164_t *e164)
{
    memset(e164, 0, sizeof(*e164));
}

void itu_t_e164_set_context(itu_t_e164_t *e164, const itu_t_e164_context_t *context)
{
    if(context == NULL) {
        memset(&e164->context, 0, sizeof(e164->context));
        e164->input_country_explicit = 0;
        e164->input_area_explicit = 0;
        return;
    }

    e164->context = *context;
    if(e164->context.input_mode > ITU_T_E164_INPUT_MODE_DIALING)
        e164->context.input_mode = ITU_T_E164_INPUT_MODE_NUMBER;
    e164->input_country_explicit = 0;
    e164->input_area_explicit = 0;
}

static ssize_t appendf(char *buffer, ssize_t size, ssize_t pos, const char *fmt, ...)
{
    int written;
    va_list ap;

    if(size <= 0) return pos;
    if(pos >= size) return pos;

    va_start(ap, fmt);
    written = vsnprintf(buffer + pos, size - pos, fmt, ap);
    va_end(ap);

    if(written < 0) return pos;
    return pos + written;
}

static void itu_t_e164_refresh_raw_phone(itu_t_e164_t *e164)
{
    int i;

    e164->raw_phone = 0;
    for(i = 0; i < e164->pos; i++)
        e164->raw_phone = e164->raw_phone * 10 + (e164->value[i] - '0');
}

static int starts_with(const char *value, const char *prefix)
{
    size_t len;

    if(prefix == NULL || prefix[0] == 0)
        return 0;

    len = strlen(prefix);
    return strncmp(value, prefix, len) == 0;
}

static size_t append_digits(char *digits, size_t size, size_t pos, const char *value)
{
    while(*value != 0 && pos < size - 1)
        digits[pos++] = *value++;

    digits[pos] = 0;
    return pos;
}

static size_t append_number(char *digits, size_t size, size_t pos, unsigned long value)
{
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "%lu", value);
    return append_digits(digits, size, pos, buffer);
}

static size_t number_length(unsigned long value)
{
    size_t len = 1;

    while(value >= 10) {
        value /= 10;
        len++;
    }

    return len;
}

static int parse_fixed_number(const char *value, int len)
{
    int parsed = 0;
    int i;

    for(i = 0; i < len; i++) {
        if(!isdigit((unsigned char)value[i]))
            return -1;
        parsed = parsed * 10 + value[i] - '0';
    }

    return parsed;
}

static int carrier_code_valid(int country_code, const char *value, int len)
{
    int carrier_code;

    carrier_code = parse_fixed_number(value, len);
    if(carrier_code < 0)
        return 0;

    return itu_t_e164_cc_has_carrier_code(country_code, carrier_code);
}

static char alpha_to_digit(int ch)
{
    ch = toupper((unsigned char)ch);

    if(ch >= 'A' && ch <= 'C') return '2';
    if(ch >= 'D' && ch <= 'F') return '3';
    if(ch >= 'G' && ch <= 'I') return '4';
    if(ch >= 'J' && ch <= 'L') return '5';
    if(ch >= 'M' && ch <= 'O') return '6';
    if(ch >= 'P' && ch <= 'S') return '7';
    if(ch >= 'T' && ch <= 'V') return '8';
    if(ch >= 'W' && ch <= 'Z') return '9';

    return 0;
}

static int input_has_explicit_country(const itu_t_e164_t *e164, const char *value, int explicit_international)
{
    char country[8];
    const char *prefix;

    if(explicit_international)
        return 1;

    if(e164->context.country_code == 0)
        return 1;

    prefix = itu_t_e164_cc_2_international_prefix(e164->context.country_code);
    if(starts_with(value, prefix))
        return 1;

    snprintf(country, sizeof(country), "%lu", (unsigned long)e164->context.country_code);
    return starts_with(value, country);
}

static int input_has_explicit_area(const itu_t_e164_t *e164, const char *value, int explicit_area)
{
    char area[16];
    const char *prefix;

    if(explicit_area)
        return 1;

    if(e164->context.area_code == 0)
        return 1;

    snprintf(area, sizeof(area), "%lu", (unsigned long)e164->context.area_code);
    if(starts_with(value, area))
        return 1;

    prefix = itu_t_e164_cc_2_national_prefix(e164->context.country_code);
    if(starts_with(value, prefix))
        return starts_with(value + strlen(prefix), area);

    return 0;
}

static void update_input_flags(itu_t_e164_t *e164, const char *value, int explicit_international, int explicit_area)
{
    e164->input_country_explicit = 0;
    e164->input_area_explicit = 0;

    if(value[0] == 0)
        return;

    e164->input_country_explicit = input_has_explicit_country(e164, value, explicit_international);
    e164->input_area_explicit = input_has_explicit_area(e164, value, explicit_area);
}

static size_t normalize_context_digits(const itu_t_e164_t *e164, const char *value, char *digits, size_t size, int explicit_area)
{
    const char *prefix;
    char area_code[16];
    size_t pos = 0;

    digits[0] = 0;
    if(value[0] == 0 || e164->context.country_code == 0)
        return append_digits(digits, size, pos, value);

    prefix = itu_t_e164_cc_2_international_prefix(e164->context.country_code);
    if(starts_with(value, prefix))
        return append_digits(digits, size, pos, value + strlen(prefix));

    pos = append_number(digits, size, pos, e164->context.country_code);

    prefix = itu_t_e164_cc_2_national_prefix(e164->context.country_code);
    if(starts_with(value, prefix))
        return append_digits(digits, size, pos, value + strlen(prefix));

    if(e164->context.area_code != 0 && !explicit_area) {
        snprintf(area_code, sizeof(area_code), "%lu", (unsigned long)e164->context.area_code);
        if(!starts_with(value, area_code))
            pos = append_digits(digits, size, pos, area_code);
    }

    return append_digits(digits, size, pos, value);
}

static size_t normalize_dialing_digits(const itu_t_e164_t *e164, const char *value, char *digits, size_t size, int skip_carrier)
{
    const char *prefix;
    char area_code[16];
    size_t pos = 0;
    int carrier_len;

    digits[0] = 0;
    if(value[0] == 0)
        return 0;

    if(e164->context.country_code == 0 || e164->context.area_code == 0)
        return 0;

    prefix = itu_t_e164_cc_2_international_prefix(e164->context.country_code);
    if(starts_with(value, prefix)) {
        value += strlen(prefix);
        carrier_len = itu_t_e164_cc_2_carrier_code_length(e164->context.country_code);
        if(carrier_len > 0 && skip_carrier && (int)strlen(value) >= carrier_len) {
            if(carrier_code_valid(e164->context.country_code, value, carrier_len))
                value += carrier_len;
        }
        if(value[0] == 0)
            return 0;
        return append_digits(digits, size, pos, value);
    }

    pos = append_number(digits, size, pos, e164->context.country_code);

    prefix = itu_t_e164_cc_2_national_prefix(e164->context.country_code);
    if(starts_with(value, prefix)) {
        value += strlen(prefix);
        carrier_len = itu_t_e164_cc_2_carrier_code_length(e164->context.country_code);
        if(carrier_len > 0 && skip_carrier && (int)strlen(value) >= carrier_len) {
            if(carrier_code_valid(e164->context.country_code, value, carrier_len))
                value += carrier_len;
        }
        if(value[0] == 0)
            return 0;
        return append_digits(digits, size, pos, value);
    }

    snprintf(area_code, sizeof(area_code), "%lu", (unsigned long)e164->context.area_code);
    if(starts_with(value, area_code)) {
        digits[0] = 0;
        return 0;
    }

    pos = append_digits(digits, size, pos, area_code);
    return append_digits(digits, size, pos, value);
}

static void itu_t_e164_truncate(itu_t_e164_t *e164, int pos)
{
    if(pos < 0) pos = 0;
    if(pos > (int)sizeof(e164->value) - 1)
        pos = sizeof(e164->value) - 1;

    e164->pos = pos;
    e164->value[e164->pos] = 0;
    itu_t_e164_refresh_raw_phone(e164);
}

static int itu_t_e164_cc_subscriber_check(int country_code, const char *ndc, const char *sn, const char **mask, enum itu_t_e164_number_kind_enum *kind)
{
    int result;
    regex_t regex_ndc;
    regex_t regex_sn;
    struct cc_regex *cc_regex;

    *mask = NULL;
    *kind = ITU_T_E164_NUMBER_KIND_UNKNOWN;
    if((cc_regex = itu_t_e164_cc_subscriber_regex(country_code)) == NULL) return 0;

    result = 1;
    while(cc_regex->regex_sn) {
        // Check NDC first if available
        if(ndc && cc_regex->regex_ndc) {
            if((result = regcomp(&regex_ndc, cc_regex->regex_ndc, REG_EXTENDED)) != 0)
                errx(EXIT_FAILURE, "Error compiling regex '%s'", cc_regex->regex_ndc);
            result = regexec(&regex_ndc, ndc, 0, NULL, 0);
            regfree(&regex_ndc);
            if(result == 1) {
                cc_regex++;
                continue;
            }
        }

        // Check for SN
        if((result = regcomp(&regex_sn, cc_regex->regex_sn, REG_EXTENDED)) != 0)
            errx(EXIT_FAILURE, "Error compiling regex '%s'", cc_regex->regex_sn);
        result = regexec(&regex_sn, sn, 0, NULL, 0);
        regfree(&regex_sn);
        *mask = cc_regex->mask_sn;
        *kind = cc_regex->kind;
        cc_regex++;
        if(result == 0) break;
    }
    return (result == 0);
}

// Update e164 helper information for numbers
static void itu_t_e164_update_number(itu_t_e164_t *e164)
{
    char area_code[16];
    int area_digits;
    const char *sn;

    // This function must be never called for other data.
    // Try to protect from invalid calls
    if(e164->cc.type != ITU_T_NUMBER) return;

    // Current number of digits discounting country code digits
    area_digits = e164->pos - e164->cc.len;

    // Resolve NDC for a maximum of four elements
    // Make sure all the digits are reacheable through the hierarch
    e164->number.ndc = 0;
    e164->number.ndc_len = 0;
    e164->number.kind = ITU_T_E164_NUMBER_KIND_UNKNOWN;
    e164->number.type = itu_t_e164_area_2_type(e164->cc.value, e164->number.ndc);
    if(e164->number.type == ITU_T_AREA_UNKNOWN) {
        e164->number.type = ITU_T_AREA_NUMBER;
        e164->number.sn_len = area_digits;
        e164->number.sn = atol(&e164->value[e164->cc.len]);
        e164->number.mask = NULL;
        e164->number.kind = ITU_T_E164_NUMBER_KIND_REGULAR;
        return;
    }

    if((area_digits > 0) && (e164->number.type == ITU_T_AREA_INCOMPLETE)) {
        e164->number.ndc = e164->value[e164->cc.len] - '0';
        e164->number.ndc_len++;
        if((area_digits > 1) && (itu_t_e164_area_2_type(e164->cc.value, e164->number.ndc) == ITU_T_AREA_INCOMPLETE)) {
            e164->number.ndc = e164->number.ndc * 10 + e164->value[e164->cc.len + 1] - '0';
            e164->number.ndc_len++;
            if((area_digits > 2) && (itu_t_e164_area_2_type(e164->cc.value, e164->number.ndc) == ITU_T_AREA_INCOMPLETE)) {
                e164->number.ndc = e164->number.ndc * 10 + e164->value[e164->cc.len + 2] - '0';
                e164->number.ndc_len++;
                if((area_digits > 3) && (itu_t_e164_area_2_type(e164->cc.value, e164->number.ndc) == ITU_T_AREA_INCOMPLETE)) {
                    e164->number.ndc = e164->number.ndc * 10 + e164->value[e164->cc.len + 3] - '0';
                    e164->number.ndc_len++;
                }
            }
        }
    }

    // Update number type
    e164->number.type = itu_t_e164_area_2_type(e164->cc.value, e164->number.ndc);

    if(e164->number.type == ITU_T_AREA_UNKNOWN) {
        if(e164->number.ndc_len > 0) {
            e164->number.ndc_len--;
            e164->number.ndc /= 10;
            // If there still some data, show incomplete
            if(e164->number.ndc_len > 0)
                e164->number.type = ITU_T_AREA_INCOMPLETE;
        }
        e164->number.sn_len = 0;
        itu_t_e164_truncate(e164, e164->cc.len + e164->number.ndc_len);
    } else {
        e164->number.sn_len = e164->pos - e164->cc.len - e164->number.ndc_len;
        e164->number.sn = atol(&e164->value[e164->cc.len + e164->number.ndc_len]);
        itu_t_e164_truncate(e164, e164->cc.len + e164->number.ndc_len + e164->number.sn_len);
    }

    if(e164->number.ndc)
        snprintf(area_code, sizeof(area_code), "%u", e164->number.ndc);

    sn = &e164->value[e164->cc.len + e164->number.ndc_len];
    while(e164->number.sn_len > 0 && itu_t_e164_cc_subscriber_check(e164->cc.value, e164->number.ndc?area_code:NULL, sn, &e164->number.mask, &e164->number.kind) == 0) {
        e164->number.sn_len--;
        itu_t_e164_truncate(e164, e164->pos - 1);
        sn = &e164->value[e164->cc.len + e164->number.ndc_len];
    }
}

// Update e164 helper information for country codes
static void itu_t_e164_update_cc(itu_t_e164_t *e164)
{
    // If it is an empty string, just set as UNKNOWN and leave
    if(e164->pos == 0) {
        e164->cc.type = ITU_T_UNKNOWN;
        return;
    }

    // Resolve CC for a maximum of three elements
    // Make sure all the digits are reacheable through the hierarch
    e164->cc.value = e164->value[0] - '0';
    e164->cc.len = 1;
    if((e164->pos > 1) && itu_t_e164_cc_2_type(e164->cc.value) == ITU_T_INCOMPLETE) {
        e164->cc.value = e164->cc.value * 10 + e164->value[1] - '0';
        e164->cc.len++;
        if((e164->pos > 2) && itu_t_e164_cc_2_type(e164->cc.value) == ITU_T_INCOMPLETE) {
            e164->cc.value = e164->cc.value * 10 + e164->value[2] - '0';
            e164->cc.len++;
        }
    }
    e164->cc.type = itu_t_e164_cc_2_type(e164->cc.value);

    // Remove invalid trailling data
    if((e164->cc.type == ITU_T_UNKNOWN) || (e164->cc.type == ITU_T_SPARE)) {
        itu_t_e164_truncate(e164, e164->cc.len);
    }
}

// Update all e164 structure when something changes
static void itu_t_e164_update(itu_t_e164_t *e164)
{
    itu_t_e164_update_cc(e164);
    switch(e164->cc.type) {
        case ITU_T_NUMBER:
            itu_t_e164_update_number(e164);
            break;
        default:
            return;
    }
}

static int context_rejects_value(itu_t_e164_t *e164)
{
    if(e164->pos == 0 || e164->context.restriction == ITU_T_E164_CONTEXT_RESTRICT_NONE)
        return 0;

    if(e164->context.country_code != 0 && e164->cc.value != e164->context.country_code)
        return 1;

    if(e164->context.restriction < ITU_T_E164_CONTEXT_RESTRICT_AREA || e164->context.area_code == 0)
        return 0;

    return e164->number.ndc_len > 0 && e164->number.ndc != e164->context.area_code;
}

// Define a new value
void itu_t_e164_set_value(itu_t_e164_t *e164, const char *value)
{
    const char *p;
    const char *national_prefix;
    itu_t_e164_context_t context;
    uint8_t input_country_explicit;
    uint8_t input_area_explicit;
    char input[32];
    char digits[sizeof(e164->value)];
    size_t pos = 0;
    int explicit_international;
    int explicit_area;
    int dialing_mode;
    int national_dialing;
    int carrier_len;

    if(value == NULL) {
        context = e164->context;
        itu_t_e164_init(e164);
        e164->context = context;
        return;
    }

    explicit_international = value[0] == '+';
    explicit_area = value[0] == '(';

    if(explicit_international || explicit_area)
        p = &value[1];
    else p = value;

    while(*p && pos < sizeof(input) - 1) {
        if(isdigit((unsigned char)*p))
            input[pos++] = *p;
        else if(e164->context.accept_alphanumeric && isalpha((unsigned char)*p))
            input[pos++] = alpha_to_digit(*p);
        p++;
    }
    input[pos] = 0;

    context = e164->context;
    dialing_mode = e164->context.input_mode == ITU_T_E164_INPUT_MODE_DIALING;
    national_prefix = itu_t_e164_cc_2_national_prefix(e164->context.country_code);
    carrier_len = itu_t_e164_cc_2_carrier_code_length(e164->context.country_code);
    national_dialing = dialing_mode && starts_with(input, national_prefix);
    update_input_flags(e164, input, explicit_international, explicit_area);
    input_country_explicit = e164->input_country_explicit;
    input_area_explicit = e164->input_area_explicit;
    if(dialing_mode)
        normalize_dialing_digits(e164, input, digits, sizeof(digits), 1);
    else if(explicit_international)
        memcpy(digits, input, pos + 1);
    else
        normalize_context_digits(e164, input, digits, sizeof(digits), explicit_area);

    itu_t_e164_init(e164);
    e164->context = context;
    e164->input_country_explicit = input_country_explicit;
    e164->input_area_explicit = input_area_explicit;
    memcpy(e164->value, digits, strlen(digits) + 1);
    e164->pos = strlen(digits);
    itu_t_e164_update(e164);
    if(national_dialing && carrier_len > 0) {
        itu_t_e164_t candidate;
        char fallback_digits[sizeof(e164->value)];

        normalize_dialing_digits(e164, input, fallback_digits, sizeof(fallback_digits), 0);
        itu_t_e164_init(&candidate);
        candidate.context = context;
        candidate.input_country_explicit = input_country_explicit;
        candidate.input_area_explicit = input_area_explicit;
        memcpy(candidate.value, fallback_digits, strlen(fallback_digits) + 1);
        candidate.pos = strlen(fallback_digits);
        itu_t_e164_update(&candidate);
        if(candidate.number.type == ITU_T_AREA_NUMBER &&
           candidate.number.kind != ITU_T_E164_NUMBER_KIND_UNKNOWN &&
           candidate.number.kind != ITU_T_E164_NUMBER_KIND_REGULAR) {
            *e164 = candidate;
        }
    }
    if(context_rejects_value(e164))
        itu_t_e164_truncate(e164, 0);
}

int print_mask(char *str, ssize_t size, const char *mask, const char *number)
{
    int str_pos = 0;
    int number_pos = 0;
    int mask_pos = 0;

    if(size <= 0) return 0;

    while(mask[mask_pos] && number[number_pos] && str_pos < size - 1) {
        if(mask[mask_pos] == '#') {
            str[str_pos] = number[number_pos++];
        } else {
            str[str_pos] = mask[mask_pos];
        }
        mask_pos++;
        str_pos++;
    }
    str[str_pos] = 0;
    return str_pos;
}

static int number_uses_area_parentheses(const itu_t_e164_t *e164)
{
    return e164->number.kind == ITU_T_E164_NUMBER_KIND_REGULAR;
}

ssize_t itu_t_e164_get_value(itu_t_e164_t *e164, char *buffer, ssize_t size)
{
    ssize_t pos;
    int has_ndc;
    int parenthesized_area;

    if(size <= 0) return 0;

    pos = appendf(buffer, size, 0, "+%.*s", e164->cc.len, &e164->value[0]);
    has_ndc = e164->number.ndc_len > 0;
    parenthesized_area = has_ndc && number_uses_area_parentheses(e164);

    if((e164->cc.type != ITU_T_INCOMPLETE) && (e164->cc.type != ITU_T_UNKNOWN)) {
        if(parenthesized_area)
            pos = appendf(buffer, size, pos, " (");
        else if(has_ndc)
            pos = appendf(buffer, size, pos, " ");

        if(e164->number.type != ITU_T_AREA_UNKNOWN) {
            if(has_ndc)
                pos = appendf(buffer, size, pos, "%u", e164->number.ndc);

            if(e164->number.type == ITU_T_AREA_NUMBER) {
                if(parenthesized_area)
                    pos = appendf(buffer, size, pos, ")");
                pos = appendf(buffer, size, pos, " ");

                if(e164->number.sn_len > 0) {
                    char number[16];
                    snprintf(number, sizeof(number), "%.*s", e164->number.sn_len, &e164->value[e164->cc.len + e164->number.ndc_len]);
                    if(e164->number.mask == NULL)
                        pos = appendf(buffer, size, pos, "%s", number);
                    else if(pos < size)
                        pos += print_mask(buffer + pos, size - pos, e164->number.mask, number);
                }
            }
        }
    }
    return pos;
}

ssize_t itu_t_e164_get_context_value(itu_t_e164_t *e164, char *buffer, ssize_t size)
{
    char full[BUFSIZ];
    const char *display = full;
    char prefix[24];
    int country_prefix_removed = 0;
    uint8_t restriction;

    if(size <= 0) return 0;
    if(e164->pos == 0) {
        buffer[0] = 0;
        return 0;
    }

    itu_t_e164_get_value(e164, full, sizeof(full));
    restriction = e164->context.restriction;

    if(e164->context.country_code != 0 &&
       (restriction >= ITU_T_E164_CONTEXT_RESTRICT_COUNTRY || !e164->input_country_explicit)) {
        snprintf(prefix, sizeof(prefix), "+%lu", (unsigned long)e164->context.country_code);
        if(starts_with(display, prefix)) {
            display += strlen(prefix);
            if(display[0] == ' ')
                display++;
            country_prefix_removed = 1;
        }
    }

    if(country_prefix_removed && !number_uses_area_parentheses(e164)) {
        const char *national_prefix = itu_t_e164_cc_2_national_prefix(e164->cc.value);

        if(national_prefix != NULL && national_prefix[0] != 0)
            return appendf(buffer, size, 0, "%s%s", national_prefix, display);
    }

    if(e164->context.area_code != 0 &&
       (restriction >= ITU_T_E164_CONTEXT_RESTRICT_AREA || !e164->input_area_explicit)) {
        snprintf(prefix, sizeof(prefix), "(%lu)", (unsigned long)e164->context.area_code);
        if(starts_with(display, prefix)) {
            display += strlen(prefix);
            if(display[0] == ' ')
                display++;
        }
    }

    return appendf(buffer, size, 0, "%s", display);
}

ssize_t itu_t_e164_get_dialing_value(itu_t_e164_t *e164, char *buffer, ssize_t size)
{
    const char *prefix;
    int carrier_len;

    if(size <= 0)
        return 0;

    buffer[0] = 0;
    if(e164->pos == 0)
        return 0;

    if(e164->context.country_code != 0 && e164->context.area_code != 0 &&
       e164->cc.value == e164->context.country_code) {
        if(e164->number.ndc_len > 0 && e164->number.ndc == e164->context.area_code &&
           number_uses_area_parentheses(e164)) {
            return appendf(buffer, size, 0, "%s",
                           &e164->value[e164->cc.len + e164->number.ndc_len]);
        }

        prefix = itu_t_e164_cc_2_national_prefix(e164->cc.value);
        if(prefix != NULL && prefix[0] != 0) {
            carrier_len = itu_t_e164_cc_2_carrier_code_length(e164->cc.value);
            if(carrier_len > 0 && number_uses_area_parentheses(e164)) {
                if(e164->context.carrier_code == 0 || number_length(e164->context.carrier_code) != (size_t)carrier_len ||
                   !itu_t_e164_cc_has_carrier_code(e164->cc.value, e164->context.carrier_code))
                    return 0;
                return appendf(buffer, size, 0, "%s%lu%s", prefix,
                               (unsigned long)e164->context.carrier_code, &e164->value[e164->cc.len]);
            }
            return appendf(buffer, size, 0, "%s%s", prefix, &e164->value[e164->cc.len]);
        }
    }

    if(e164->context.country_code != 0) {
        prefix = itu_t_e164_cc_2_international_prefix(e164->context.country_code);
        if(prefix != NULL && prefix[0] != 0) {
            carrier_len = itu_t_e164_cc_2_carrier_code_length(e164->context.country_code);
            if(carrier_len > 0) {
                if(e164->context.carrier_code == 0 || number_length(e164->context.carrier_code) != (size_t)carrier_len ||
                   !itu_t_e164_cc_has_carrier_code(e164->context.country_code, e164->context.carrier_code))
                    return 0;
                return appendf(buffer, size, 0, "%s%lu%s", prefix,
                               (unsigned long)e164->context.carrier_code, e164->value);
            }
            return appendf(buffer, size, 0, "%s%s", prefix, e164->value);
        }
    }

    return appendf(buffer, size, 0, "+%s", e164->value);
}

int itu_t_e164_add_digit(itu_t_e164_t *e164, char digit)
{
    if(e164->pos >= 15) return 0;
    if(isdigit(digit) == 0) return 0;
    e164->value[e164->pos++] = digit;
    e164->value[e164->pos] = 0;
    e164->raw_phone = e164->raw_phone * 10 + ( digit - '0' );
    itu_t_e164_update(e164);
    return 1;
}

int itu_t_e164_del_digit(itu_t_e164_t *e164)
{
    if(e164->pos == 0) return 0;
    e164->value[--e164->pos] = 0;
    e164->raw_phone /= 10;
    itu_t_e164_update(e164);
    return 1;
}
