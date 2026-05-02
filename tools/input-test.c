#include <assert.h>
#include <string.h>

#define ITUTE164_INPUT_NO_MAIN
#include "input.c"

static void set_text_field(char *field, size_t *len, size_t size, const char *value)
{
    snprintf(field, size, "%s", value);
    *len = strlen(field);
}

static void test_dialing_rejects_trailing_zeroes(void)
{
    itu_t_e164_t e164;
    struct input_state state;
    const char *digits = "005519982592222000000000000000";
    size_t i;

    assert(itu_t_e164_load_plan_file("../data/e164-plan.txt") == 0);

    memset(&state, 0, sizeof(state));
    state.field = FIELD_PHONE;
    set_text_field(state.country, &state.country_len, sizeof(state.country), "55");
    set_text_field(state.area, &state.area_len, sizeof(state.area), "19");
    set_text_field(state.carrier, &state.carrier_len, sizeof(state.carrier), "21");
    set_text_field(state.mode, &state.mode_len, sizeof(state.mode), "1");

    itu_t_e164_init(&e164);
    apply_context(&e164, &state);
    for(i = 0; digits[i] != 0; i++)
        handle_phone_key(&state, &e164, digits[i]);

    assert(strcmp(state.phone, "005519982592222") == 0);
    assert(strcmp(e164.value, "5519982592222") == 0);

    itu_t_e164_reset_plan();
}

int main(void)
{
    test_dialing_rejects_trailing_zeroes();
    return 0;
}
