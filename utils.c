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
        return;
    }

    e164->context = *context;
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

static size_t normalize_context_digits(const itu_t_e164_t *e164, const char *value, char *digits, size_t size)
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

    if(e164->context.area_code != 0) {
        snprintf(area_code, sizeof(area_code), "%lu", (unsigned long)e164->context.area_code);
        if(!starts_with(value, area_code))
            pos = append_digits(digits, size, pos, area_code);
    }

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

static int itu_t_e164_cc_subscriber_check(int country_code, const char *ndc, const char *sn, const char **mask)
{
    int result;
    regex_t regex_ndc;
    regex_t regex_sn;
    struct cc_regex *cc_regex;

    *mask = NULL;
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
    e164->number.type = itu_t_e164_area_2_type(e164->cc.value, e164->number.ndc);
    if(e164->number.type == ITU_T_AREA_UNKNOWN) {
        e164->number.type = ITU_T_AREA_NUMBER;
        e164->number.sn_len = area_digits;
        e164->number.sn = atol(&e164->value[e164->cc.len]);
        e164->number.mask = NULL;
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
    while(e164->number.sn_len > 0 && itu_t_e164_cc_subscriber_check(e164->cc.value, e164->number.ndc?area_code:NULL, sn, &e164->number.mask) == 0) {
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

// Define a new value
void itu_t_e164_set_value(itu_t_e164_t *e164, const char *value)
{
    const char *p;
    itu_t_e164_context_t context;
    char input[sizeof(e164->value)];
    char digits[sizeof(e164->value)];
    size_t pos = 0;

    if(value == NULL) {
        context = e164->context;
        itu_t_e164_init(e164);
        e164->context = context;
        return;
    }

    if(value[0] == '+')
        p = &value[1];
    else
        p = value;

    while(*p && pos < sizeof(digits) - 1) {
        if(isdigit((unsigned char)*p))
            input[pos++] = *p;
        p++;
    }
    input[pos] = 0;

    context = e164->context;
    if(value[0] == '+')
        memcpy(digits, input, pos + 1);
    else
        normalize_context_digits(e164, input, digits, sizeof(digits));

    itu_t_e164_init(e164);
    e164->context = context;
    memcpy(e164->value, digits, strlen(digits) + 1);
    e164->pos = strlen(digits);
    itu_t_e164_update(e164);
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

ssize_t itu_t_e164_get_value(itu_t_e164_t *e164, char *buffer, ssize_t size)
{
    ssize_t pos;
    int has_ndc;

    if(size <= 0) return 0;

    pos = appendf(buffer, size, 0, "+%.*s", e164->cc.len, &e164->value[0]);
    has_ndc = e164->number.ndc_len > 0;

    if((e164->cc.type != ITU_T_INCOMPLETE) && (e164->cc.type != ITU_T_UNKNOWN)) {
        if(has_ndc)
            pos = appendf(buffer, size, pos, " (");

        if(e164->number.type != ITU_T_AREA_UNKNOWN) {
            if(has_ndc)
                pos = appendf(buffer, size, pos, "%u", e164->number.ndc);

            if(e164->number.type == ITU_T_AREA_NUMBER) {
                if(has_ndc)
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
