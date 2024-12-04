#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <err.h>
#include "libitute164.h"

// Initialize structure
void itu_t_e164_init(itu_t_e164_t *e164)
{
    bzero(e164, sizeof(*e164));
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
    // This function must be never called for other data.
    // Try to protect from invalid calls
    if(e164->cc.type != ITU_T_NUMBER) return;

    // Current number of digits discounting country code digits
    int area_digits = e164->pos - e164->cc.len;

    // Resolve NDC for a maximum of four elements
    // Make sure all the digits are reacheable through the hierarch
    e164->number.ndc = 0;
    e164->number.ndc_len = 0;
    if(itu_t_e164_area_2_type(e164->cc.value, e164->number.ndc) == ITU_T_AREA_INCOMPLETE) {
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
        e164->pos = e164->cc.len + e164->number.ndc_len;
    } else {
        e164->number.sn_len = e164->pos - e164->cc.len - e164->number.ndc_len;
        e164->number.sn = atol(&e164->value[e164->cc.len + e164->number.ndc_len]);
        e164->pos = e164->cc.len + e164->number.ndc_len + e164->number.sn_len;
    }

    // Truncate buffer to the expected number of digits
    e164->value[e164->pos] = 0;

    char area_code[8];
    if(e164->number.ndc)
        snprintf(area_code, sizeof(area_code), "%u", e164->number.ndc);

    if(itu_t_e164_cc_subscriber_check(e164->cc.value, e164->number.ndc?area_code:NULL, &e164->value[e164->cc.len + e164->number.ndc_len], &e164->number.mask) == 0) {
        itu_t_e164_del_digit(e164);
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
        e164->pos = e164->cc.len;
        e164->value[e164->pos] = 0;
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
    if(value[0] == '+')
        p = &value[1];
    else
        p = value;

    strcpy(e164->value, p);
    e164->pos = strlen(e164->value);
    itu_t_e164_update(e164);
}

int print_mask(char *str, ssize_t size, const char *mask, const char *number)
{
    int str_pos = 0;
    int number_pos = 0;
    int mask_pos = 0;

    while(mask[mask_pos] && number[number_pos]) {
        if(mask[mask_pos] == '#') {
            str[str_pos] = number[number_pos++];
        } else {
            str[str_pos] = mask[mask_pos];
        }
        mask_pos++;
        str_pos++;
        if(str_pos > size) break;
    }
    str[str_pos] = 0;
    return str_pos;
}

ssize_t itu_t_e164_get_value(itu_t_e164_t *e164, char *buffer, ssize_t size)
{
    int pos;
    pos = snprintf(buffer, size, "+%.*s", e164->cc.len, &e164->value[0]);

    if((e164->cc.type != ITU_T_INCOMPLETE) && (e164->cc.type != ITU_T_UNKNOWN)) {
        // Only shows something if there is area code in this country code
        if(!((e164->number.type == ITU_T_AREA_NUMBER) && (e164->number.ndc_len == 0)))
            pos += snprintf(buffer + pos, size - pos, " (");

        //if((e164->number.type != ITU_T_AREA_UNKNOWN) && (e164->number.ndc_len > 0)) {
        if(e164->number.type != ITU_T_AREA_UNKNOWN) {
            // Only shows something if there is area code in this country code
            if(!((e164->number.type == ITU_T_AREA_NUMBER) && (e164->number.ndc_len == 0)))
                pos += snprintf(buffer + pos, size - pos, "%u", e164->number.ndc);

            if(e164->number.type == ITU_T_AREA_NUMBER) {
                // Only shows something if there is area code in this country code
                if(!((e164->number.type == ITU_T_AREA_NUMBER) && (e164->number.ndc_len == 0)))
                    pos += snprintf(buffer + pos, size - pos, ")");
                pos += snprintf(buffer + pos, size - pos, " ");

                if(e164->number.sn_len > 0) {
                    char number[16];
                    sprintf(number, "%lu", e164->number.sn);
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
