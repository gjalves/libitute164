#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <cmocka.h>
#include "libitute164.h"

static int total_cc_unused = 0;
static int total_cc_used = 0;

static int cc_2_type_setup(void **state)
{
    int *value;
    *state = malloc(sizeof(int));
    value = *state;
    *value = 0;
    return 0;
}

static int cc_2_type_teardown(void **state)
{
    free(*state);
    return 0;
}

static void cc_2_type_test_used(void **state)
{
    (void) state;
    int status;
    int i;
    int *level = *state;
    int last_level;

    for(i = 0 + *level; i < 10 + *level; i++) {
        switch((status = itu_t_e164_cc_2_type(i))) {
            case ITU_T_UNKNOWN:
                fail_msg("Missing entry for country code %u", i);
            case ITU_T_INCOMPLETE:
                last_level = *level;
                *level = i * 10;
                cc_2_type_test_used(state);
                *level = last_level;
                /* fall through */
            case ITU_T_RESERVED:
                total_cc_used++;
                break;
            case ITU_T_SPARE:
                total_cc_used++;
                break;
            case ITU_T_NUMBER:
                total_cc_used++;
                break;
            case ITU_T_GLOBAL:
                total_cc_used++;
                break;
            case ITU_T_GROUPS:
                total_cc_used++;
                break;
            case ITU_T_TRIALS:
                total_cc_used++;
                break;
            default:
                fail_msg("Unknown status for function itu_t_e164_cc_2_type(%u): %u", i, status);
        }
    }
}

static void cc_2_type_test_total(void **state)
{
    (void) state;
    assert_int_equal(1000, total_cc_unused + total_cc_used);
}

static void e164_test_append_valid(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_add_digit(&e164, '5');
    itu_t_e164_add_digit(&e164, '5');
    itu_t_e164_add_digit(&e164, '1');
    itu_t_e164_add_digit(&e164, '9');
    itu_t_e164_add_digit(&e164, '1');
    itu_t_e164_add_digit(&e164, '2');
    itu_t_e164_add_digit(&e164, '3');
    itu_t_e164_add_digit(&e164, '4');
    itu_t_e164_add_digit(&e164, '5');
    itu_t_e164_add_digit(&e164, '6');
    itu_t_e164_add_digit(&e164, '7');
    itu_t_e164_add_digit(&e164, '8');
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-5678", buffer);
    assert_string_equal("551912345678", e164.value);
}

static void e164_test_append_spare_cc(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_add_digit(&e164, '8');
    itu_t_e164_add_digit(&e164, '5');
    itu_t_e164_add_digit(&e164, '7');
    itu_t_e164_add_digit(&e164, '7');
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_true(e164.pos == 3);
    assert_string_equal("+857", buffer);
    assert_string_equal("857", e164.value);
    assert_int_equal(857, e164.raw_phone);
}

static void e164_test_append_invalid_cc(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_add_digit(&e164, '2');
    itu_t_e164_add_digit(&e164, '1');
    itu_t_e164_add_digit(&e164, '4');
    itu_t_e164_add_digit(&e164, '1');
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_true(e164.pos == 3);
    assert_string_equal("+214", buffer);
    assert_string_equal("214", e164.value);
}

static void e164_test_chop_normal(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+551912345678");
    itu_t_e164_del_digit(&e164);
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-567", buffer);
}

// Remove with chop until became empty and try to fill it again
static void e164_test_chop_empty(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_add_digit(&e164, '5');
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+5", buffer);
    itu_t_e164_del_digit(&e164);
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+", buffer);
    itu_t_e164_add_digit(&e164, '5');
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+5", buffer);
}

// Try to chop an empty value
static void e164_test_chop_overshooting(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_del_digit(&e164);
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_true(e164.pos == 0);
    assert_string_equal("+", buffer);
}

static void e164_test_set_area_0(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+59812345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+598 1 234 5678", buffer);
}

static void e164_test_set_area_2(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+551912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-5678", buffer);
}

static void e164_test_set_area_3(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+14691234567");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+1 (469) 123-4567", buffer);
}

static void e164_test_set_area_4(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+542966123456");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+54 (2966) 123456", buffer);
}

static void e164_test_set_formatted_value(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+55 (19) 1234-5678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-5678", buffer);
    assert_string_equal("551912345678", e164.value);
}

static void e164_test_set_long_value(void **state)
{
    (void) state;
    itu_t_e164_t e164;

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+551912345678999999");
    assert_true((size_t)e164.pos < sizeof(e164.value));
    assert_string_equal("551912345678", e164.value);
}

static void e164_test_small_buffer(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[4];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+551912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55", buffer);
}

static void e164_test_preserve_subscriber_zeroes(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+12010001234");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+1 (201) 000-1234", buffer);
}

static void e164_test_country_without_mask(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+31123456");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+31 123456", buffer);
    assert_string_equal("31123456", e164.value);
}

static void e164_test_set_null_value(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+551912345678");
    itu_t_e164_set_value(&e164, NULL);
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_true(e164.pos == 0);
    assert_string_equal("+", buffer);
}

static void e164_test_load_plan_file(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    assert_int_equal(0, itu_t_e164_load_plan_file("data/e164-plan.txt"));
    assert_string_equal("pt_BR", itu_t_e164_cc_2_country(55));
    assert_string_equal("0", itu_t_e164_cc_2_national_prefix(55));
    assert_string_equal("00", itu_t_e164_cc_2_international_prefix(55));

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+55");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55", buffer);

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+551912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-5678", buffer);
    itu_t_e164_reset_plan();
}

static void e164_test_context_local_and_national_input(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    itu_t_e164_context_t open_context = {55, 19, ITU_T_E164_CONTEXT_RESTRICT_NONE};
    itu_t_e164_context_t country_context = {55, 0, ITU_T_E164_CONTEXT_RESTRICT_COUNTRY};
    itu_t_e164_context_t area_context = {55, 19, ITU_T_E164_CONTEXT_RESTRICT_AREA};
    char buffer[BUFSIZ];

    assert_int_equal(0, itu_t_e164_load_plan_file("data/e164-plan.txt"));

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+91 2345678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+91 2345678", buffer);
    assert_string_equal("912345678", e164.value);

    itu_t_e164_init(&e164);
    itu_t_e164_set_context(&e164, &country_context);
    itu_t_e164_set_value(&e164, "19912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 91234-5678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("(19) 91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_init(&e164);
    itu_t_e164_set_context(&e164, &country_context);
    itu_t_e164_set_value(&e164, "+5519912345678");
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("(19) 91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_set_value(&e164, "+12010001234");
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("", buffer);
    assert_string_equal("", e164.value);

    itu_t_e164_init(&e164);
    itu_t_e164_set_context(&e164, &open_context);
    itu_t_e164_set_value(&e164, "");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("", buffer);
    assert_string_equal("", e164.value);

    itu_t_e164_set_value(&e164, "912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 91234-5678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_set_value(&e164, "(19912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 91234-5678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("(19) 91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_set_value(&e164, "19912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 91234-5678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("(19) 91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_set_value(&e164, "019912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 91234-5678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("(19) 91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_set_value(&e164, "+5519912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 91234-5678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_set_value(&e164, "00551912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-5678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-5678", buffer);
    assert_string_equal("551912345678", e164.value);

    itu_t_e164_set_value(&e164, "+551912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-5678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-5678", buffer);
    assert_string_equal("551912345678", e164.value);

    itu_t_e164_init(&e164);
    itu_t_e164_set_context(&e164, &area_context);
    itu_t_e164_set_value(&e164, "912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 91234-5678", buffer);
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_set_value(&e164, "(19912345678");
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_set_value(&e164, "+5519912345678");
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("91234-5678", buffer);
    assert_string_equal("5519912345678", e164.value);

    itu_t_e164_set_value(&e164, "(11912345678");
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("", buffer);
    assert_string_equal("", e164.value);

    itu_t_e164_set_value(&e164, "+5511912345678");
    itu_t_e164_get_context_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("", buffer);
    assert_string_equal("", e164.value);

    itu_t_e164_reset_plan();
}

static void e164_test_load_plan_override(void **state)
{
    (void) state;
    FILE *fp;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    fp = tmpfile();
    assert_non_null(fp);
    fprintf(fp, "cc 1 incomplete\n");
    fprintf(fp, "cc 12 incomplete\n");
    fprintf(fp, "cc 123 number\n");
    fprintf(fp, "country 123 pt_BR\n");
    fprintf(fp, "area 123 0 number\n");
    fprintf(fp, "subscriber 123 * ^[0-9]{0,3}$ ###\n");
    rewind(fp);

    assert_int_equal(0, itu_t_e164_load_plan_fp(fp));
    fclose(fp);

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+123456");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+123 456", buffer);
    assert_string_equal("123456", e164.value);
    assert_string_equal("pt_BR", itu_t_e164_cc_2_country(123));
    itu_t_e164_reset_plan();
}

static void e164_test_load_plan_invalid_keeps_active_plan(void **state)
{
    (void) state;
    FILE *fp;
    const char *error;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    fp = tmpfile();
    assert_non_null(fp);
    fprintf(fp, "cc 1 incomplete\n");
    fprintf(fp, "cc 12 incomplete\n");
    fprintf(fp, "cc 123 number\n");
    fprintf(fp, "country 123 pt_BR\n");
    fprintf(fp, "area 123 0 number\n");
    fprintf(fp, "subscriber 123 * ^[0-9]{0,3}$ ###\n");
    rewind(fp);
    assert_int_equal(0, itu_t_e164_load_plan_fp(fp));
    fclose(fp);

    fp = tmpfile();
    assert_non_null(fp);
    fprintf(fp, "cc 1000 number\n");
    rewind(fp);

    assert_int_equal(-1, itu_t_e164_load_plan_fp(fp));
    error = itu_t_e164_plan_error();
    assert_non_null(error);
    assert_non_null(strstr(error, "line 1"));
    assert_non_null(strstr(error, "invalid country code"));
    fclose(fp);

    assert_int_equal(ITU_T_NUMBER, itu_t_e164_cc_2_type(123));
    assert_string_equal("pt_BR", itu_t_e164_cc_2_country(123));
    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+123456");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+123 456", buffer);
    itu_t_e164_reset_plan();
}

static void e164_test_load_default_plan_from_env(void **state)
{
    (void) state;
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    assert_int_equal(0, setenv("LIBITUTE164_PLAN", "data/e164-plan.txt", 1));
    assert_int_equal(0, itu_t_e164_load_default_plan());
    assert_int_equal(0, unsetenv("LIBITUTE164_PLAN"));

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+59812345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+598 1 234 5678", buffer);
    itu_t_e164_reset_plan();
}

static void e164_test_load_plan_invalid_country_tag(void **state)
{
    (void) state;
    FILE *fp;
    const char *error;

    itu_t_e164_reset_plan();
    fp = tmpfile();
    assert_non_null(fp);
    fprintf(fp, "country 55 pt-BR\n");
    rewind(fp);

    assert_int_equal(-1, itu_t_e164_load_plan_fp(fp));
    error = itu_t_e164_plan_error();
    assert_non_null(error);
    assert_non_null(strstr(error, "line 1"));
    assert_non_null(strstr(error, "invalid country tag"));
    fclose(fp);
    assert_null(itu_t_e164_cc_2_country(55));
    itu_t_e164_reset_plan();
}

static void cc_2_type_count_unused(void)
{
    int i;
    for(i = 0; i < 1000; i++) {
        if(itu_t_e164_cc_2_type(i) == ITU_T_UNKNOWN) total_cc_unused++;
    }
}

int main(void) {
    int ret;
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(cc_2_type_test_used),
        cmocka_unit_test(cc_2_type_test_total),
        cmocka_unit_test(e164_test_set_area_0),
        cmocka_unit_test(e164_test_set_area_2),
        cmocka_unit_test(e164_test_set_area_3),
        cmocka_unit_test(e164_test_set_area_4),
        cmocka_unit_test(e164_test_set_formatted_value),
        cmocka_unit_test(e164_test_set_long_value),
        cmocka_unit_test(e164_test_small_buffer),
        cmocka_unit_test(e164_test_preserve_subscriber_zeroes),
        cmocka_unit_test(e164_test_country_without_mask),
        cmocka_unit_test(e164_test_set_null_value),
        cmocka_unit_test(e164_test_load_plan_file),
        cmocka_unit_test(e164_test_context_local_and_national_input),
        cmocka_unit_test(e164_test_load_plan_override),
        cmocka_unit_test(e164_test_load_plan_invalid_keeps_active_plan),
        cmocka_unit_test(e164_test_load_default_plan_from_env),
        cmocka_unit_test(e164_test_load_plan_invalid_country_tag),
        cmocka_unit_test(e164_test_append_valid),
        cmocka_unit_test(e164_test_append_spare_cc),
        cmocka_unit_test(e164_test_append_invalid_cc),
        cmocka_unit_test(e164_test_chop_normal),
        cmocka_unit_test(e164_test_chop_empty),
        cmocka_unit_test(e164_test_chop_overshooting),
    };

    cc_2_type_count_unused();
    ret = cmocka_run_group_tests(tests, cc_2_type_setup, cc_2_type_teardown);
    return ret;
}
