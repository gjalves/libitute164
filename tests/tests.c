#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "libitute164.h"

static int total_cc_unused = 0;
static int total_cc_used = 0;

static int cc_2_type_setup(void **state)
{
    int *value;
    *state = malloc(sizeof(int *));
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
    assert_int_equal(1000, total_cc_unused + total_cc_used);
}

static void e164_test_append_valid(void **state)
{
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
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_add_digit(&e164, '8');
    itu_t_e164_add_digit(&e164, '5');
    itu_t_e164_add_digit(&e164, '7');
    itu_t_e164_add_digit(&e164, '7');
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_true(e164.pos == 3);
    assert_string_equal("+857 (", buffer);
    assert_string_equal("857", e164.value);
}

static void e164_test_append_invalid_cc(void **state)
{
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_add_digit(&e164, '2');
    itu_t_e164_add_digit(&e164, '1');
    itu_t_e164_add_digit(&e164, '4');
    itu_t_e164_add_digit(&e164, '1');
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_true(e164.pos == 3);
    assert_string_equal("+214 (", buffer);
    assert_string_equal("214", e164.value);
}

static void e164_test_chop_normal(void **state)
{
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
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+59812345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+598 1 234 5678", buffer);
}

static void e164_test_set_area_2(void **state)
{
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+551912345678");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+55 (19) 1234-5678", buffer);
}

static void e164_test_set_area_3(void **state)
{
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+14691234567");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+1 (469) 123-4567", buffer);
}

static void e164_test_set_area_4(void **state)
{
    itu_t_e164_t e164;
    char buffer[BUFSIZ];

    itu_t_e164_init(&e164);
    itu_t_e164_set_value(&e164, "+542966123456");
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    assert_string_equal("+54 (2966) 123456", buffer);
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

