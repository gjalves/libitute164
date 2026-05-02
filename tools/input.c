#include <ncurses.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libitute164.h>

#define INPUT_WIDTH 72
#define PHONE_INPUT_SIZE 32

enum input_field {
    FIELD_COUNTRY,
    FIELD_AREA,
    FIELD_CARRIER,
    FIELD_RESTRICTION,
    FIELD_MODE,
    FIELD_ALPHANUMERIC,
    FIELD_PHONE,
    FIELD_TOTAL
};

struct input_state {
    char country[8];
    char area[8];
    char carrier[8];
    char restriction[2];
    char mode[2];
    char phone[PHONE_INPUT_SIZE];
    size_t country_len;
    size_t area_len;
    size_t carrier_len;
    size_t restriction_len;
    size_t mode_len;
    size_t phone_len;
    int accept_alphanumeric;
    enum input_field field;
};

static void load_numbering_plan(void)
{
    if(itu_t_e164_load_default_plan() == 0)
        return;

    itu_t_e164_load_plan_file("data/e164-plan.txt");
}

static const char *known_country(itu_t_e164_t *e164)
{
    return itu_t_e164_get_country(e164);
}

static unsigned long parse_number(const char *value)
{
    char *end;
    unsigned long parsed;

    if(value[0] == 0)
        return 0;

    parsed = strtoul(value, &end, 10);
    return *end == 0 ? parsed : 0;
}

static int e164_is_complete(const itu_t_e164_t *e164)
{
    int i;

    if(e164->pos == 0 || e164->cc.type != ITU_T_NUMBER)
        return 0;

    if(e164->number.type != ITU_T_AREA_NUMBER || e164->number.sn_len == 0)
        return 0;

    for(i = 0; i < 10; i++) {
        itu_t_e164_t copy = *e164;

        itu_t_e164_add_digit(&copy, '0' + i);
        if(strcmp(copy.value, e164->value) != 0)
            return 0;
    }

    return 1;
}

static void apply_context(itu_t_e164_t *e164, const struct input_state *state)
{
    itu_t_e164_context_t context;

    memset(&context, 0, sizeof(context));
    context.country_code = parse_number(state->country);
    if(context.country_code != 0) {
        context.area_code = parse_number(state->area);
        context.carrier_code = parse_number(state->carrier);
        context.restriction = parse_number(state->restriction);
        if(context.restriction > ITU_T_E164_CONTEXT_RESTRICT_AREA)
            context.restriction = ITU_T_E164_CONTEXT_RESTRICT_NONE;
        if(context.restriction == ITU_T_E164_CONTEXT_RESTRICT_AREA && context.area_code == 0)
            context.restriction = ITU_T_E164_CONTEXT_RESTRICT_COUNTRY;
    }
    context.accept_alphanumeric = state->accept_alphanumeric;
    context.input_mode = parse_number(state->mode);
    if(context.input_mode > ITU_T_E164_INPUT_MODE_DIALING)
        context.input_mode = ITU_T_E164_INPUT_MODE_NUMBER;
    itu_t_e164_set_context(e164, &context);
    itu_t_e164_set_value(e164, state->phone);
}

static int prefix_allowed(const itu_t_e164_t *e164, int ch)
{
    if(e164->context.input_mode == ITU_T_E164_INPUT_MODE_DIALING)
        return 0;

    if(ch == '+')
        return e164->context.restriction == ITU_T_E164_CONTEXT_RESTRICT_NONE;

    if(ch == '(')
        return e164->context.restriction < ITU_T_E164_CONTEXT_RESTRICT_AREA;

    return 0;
}

static const char *field_value(const struct input_state *state, enum input_field field)
{
    switch(field) {
        case FIELD_COUNTRY:
            return state->country;
        case FIELD_AREA:
            return state->area;
        case FIELD_CARRIER:
            return state->carrier;
        case FIELD_RESTRICTION:
            return state->restriction;
        case FIELD_MODE:
            return state->mode;
        case FIELD_ALPHANUMERIC:
            return state->accept_alphanumeric ? "sim" : "nao";
        case FIELD_PHONE:
            return state->phone;
        default:
            return "";
    }
}

static size_t *field_len(struct input_state *state, enum input_field field)
{
    switch(field) {
        case FIELD_COUNTRY:
            return &state->country_len;
        case FIELD_AREA:
            return &state->area_len;
        case FIELD_CARRIER:
            return &state->carrier_len;
        case FIELD_RESTRICTION:
            return &state->restriction_len;
        case FIELD_MODE:
            return &state->mode_len;
        case FIELD_ALPHANUMERIC:
            return &state->phone_len;
        case FIELD_PHONE:
            return &state->phone_len;
        default:
            return &state->phone_len;
    }
}

static char *field_buffer(struct input_state *state, enum input_field field)
{
    switch(field) {
        case FIELD_COUNTRY:
            return state->country;
        case FIELD_AREA:
            return state->area;
        case FIELD_CARRIER:
            return state->carrier;
        case FIELD_RESTRICTION:
            return state->restriction;
        case FIELD_MODE:
            return state->mode;
        case FIELD_ALPHANUMERIC:
            return state->phone;
        case FIELD_PHONE:
            return state->phone;
        default:
            return state->phone;
    }
}

static size_t field_size(enum input_field field)
{
    switch(field) {
        case FIELD_COUNTRY:
            return sizeof(((struct input_state *)0)->country);
        case FIELD_AREA:
            return sizeof(((struct input_state *)0)->area);
        case FIELD_CARRIER:
            return sizeof(((struct input_state *)0)->carrier);
        case FIELD_RESTRICTION:
            return sizeof(((struct input_state *)0)->restriction);
        case FIELD_MODE:
            return sizeof(((struct input_state *)0)->mode);
        case FIELD_ALPHANUMERIC:
            return 0;
        case FIELD_PHONE:
            return sizeof(((struct input_state *)0)->phone);
        default:
            return 0;
    }
}

static const char *restriction_label(const char *value)
{
    switch(parse_number(value)) {
        case ITU_T_E164_CONTEXT_RESTRICT_COUNTRY:
            return "DDI";
        case ITU_T_E164_CONTEXT_RESTRICT_AREA:
            return "DDD";
        default:
            return "aberta";
    }
}

static const char *mode_label(const char *value)
{
    switch(parse_number(value)) {
        case ITU_T_E164_INPUT_MODE_DIALING:
            return "discagem";
        default:
            return "numero";
    }
}

static unsigned long restriction_value(const struct input_state *state)
{
    return parse_number(state->restriction);
}

static unsigned long mode_value(const struct input_state *state)
{
    return parse_number(state->mode);
}

static void set_mode_value(struct input_state *state, unsigned long value)
{
    if(value > ITU_T_E164_INPUT_MODE_DIALING)
        value = ITU_T_E164_INPUT_MODE_NUMBER;

    if(value == ITU_T_E164_INPUT_MODE_NUMBER) {
        state->mode[0] = 0;
        state->mode_len = 0;
        return;
    }

    snprintf(state->mode, sizeof(state->mode), "%lu", value);
    state->mode_len = strlen(state->mode);
}

static void cycle_mode(struct input_state *state)
{
    set_mode_value(state, mode_value(state) == ITU_T_E164_INPUT_MODE_NUMBER ?
                   ITU_T_E164_INPUT_MODE_DIALING : ITU_T_E164_INPUT_MODE_NUMBER);
}

static void set_restriction_value(struct input_state *state, unsigned long value)
{
    if(value > ITU_T_E164_CONTEXT_RESTRICT_AREA)
        value = ITU_T_E164_CONTEXT_RESTRICT_NONE;

    if(value == ITU_T_E164_CONTEXT_RESTRICT_NONE) {
        state->restriction[0] = 0;
        state->restriction_len = 0;
        return;
    }

    snprintf(state->restriction, sizeof(state->restriction), "%lu", value);
    state->restriction_len = strlen(state->restriction);
}

static void cycle_restriction(struct input_state *state, int direction)
{
    unsigned long value = restriction_value(state);

    if(direction < 0)
        value = value == 0 ? ITU_T_E164_CONTEXT_RESTRICT_AREA : value - 1;
    else
        value = (value + 1) % (ITU_T_E164_CONTEXT_RESTRICT_AREA + 1);

    set_restriction_value(state, value);
}

static void draw_field(WINDOW *win, int y, const char *label, const char *value, int active)
{
    mvwprintw(win, y, 2, "%s", label);
    if(active)
        wattron(win, A_REVERSE);
    mvwprintw(win, y, 18, "%-20s", value);
    if(active)
        wattroff(win, A_REVERSE);
}

static void draw_restriction_field(WINDOW *win, int y, const struct input_state *state)
{
    unsigned long value = restriction_value(state);
    int active = state->field == FIELD_RESTRICTION;

    mvwprintw(win, y, 2, "Restricao");
    if(active && value == ITU_T_E164_CONTEXT_RESTRICT_NONE)
        wattron(win, A_REVERSE);
    mvwprintw(win, y, 18, "%s", value == ITU_T_E164_CONTEXT_RESTRICT_NONE ? "[aberta]" : " aberta ");
    if(active && value == ITU_T_E164_CONTEXT_RESTRICT_NONE)
        wattroff(win, A_REVERSE);
    if(active && value == ITU_T_E164_CONTEXT_RESTRICT_COUNTRY)
        wattron(win, A_REVERSE);
    mvwprintw(win, y, 27, "%s", value == ITU_T_E164_CONTEXT_RESTRICT_COUNTRY ? "[DDI]" : " DDI ");
    if(active && value == ITU_T_E164_CONTEXT_RESTRICT_COUNTRY)
        wattroff(win, A_REVERSE);
    if(active && value == ITU_T_E164_CONTEXT_RESTRICT_AREA)
        wattron(win, A_REVERSE);
    mvwprintw(win, y, 34, "%s", value == ITU_T_E164_CONTEXT_RESTRICT_AREA ? "[DDD]" : " DDD ");
    if(active && value == ITU_T_E164_CONTEXT_RESTRICT_AREA)
        wattroff(win, A_REVERSE);
}

static void draw_toggle_field(WINDOW *win, int y, const char *label, int enabled, int active)
{
    mvwprintw(win, y, 2, "%s", label);
    if(active && !enabled)
        wattron(win, A_REVERSE);
    mvwprintw(win, y, 18, "%s", enabled ? " nao " : "[nao]");
    if(active && !enabled)
        wattroff(win, A_REVERSE);
    if(active && enabled)
        wattron(win, A_REVERSE);
    mvwprintw(win, y, 25, "%s", enabled ? "[sim]" : " sim ");
    if(active && enabled)
        wattroff(win, A_REVERSE);
}

static void draw_mode_field(WINDOW *win, int y, const struct input_state *state)
{
    unsigned long value = mode_value(state);
    int active = state->field == FIELD_MODE;

    mvwprintw(win, y, 2, "Modo");
    if(active && value == ITU_T_E164_INPUT_MODE_NUMBER)
        wattron(win, A_REVERSE);
    mvwprintw(win, y, 18, "%s", value == ITU_T_E164_INPUT_MODE_NUMBER ? "[numero]" : " numero ");
    if(active && value == ITU_T_E164_INPUT_MODE_NUMBER)
        wattroff(win, A_REVERSE);
    if(active && value == ITU_T_E164_INPUT_MODE_DIALING)
        wattron(win, A_REVERSE);
    mvwprintw(win, y, 28, "%s", value == ITU_T_E164_INPUT_MODE_DIALING ? "[discagem]" : " discagem ");
    if(active && value == ITU_T_E164_INPUT_MODE_DIALING)
        wattroff(win, A_REVERSE);
}

static int field_y(enum input_field field)
{
    switch(field) {
        case FIELD_COUNTRY:
            return 2;
        case FIELD_AREA:
            return 3;
        case FIELD_CARRIER:
            return 4;
        case FIELD_RESTRICTION:
            return 5;
        case FIELD_MODE:
            return 6;
        case FIELD_ALPHANUMERIC:
            return 7;
        case FIELD_PHONE:
            return 9;
        default:
            return 2;
    }
}

static int field_cursor_x(const struct input_state *state)
{
    size_t len;

    if(state->field == FIELD_RESTRICTION)
        len = strlen(restriction_label(state->restriction));
    else if(state->field == FIELD_MODE)
        len = strlen(mode_label(state->mode));
    else if(state->field == FIELD_ALPHANUMERIC)
        len = strlen(field_value(state, state->field));
    else
        len = *field_len((struct input_state *)state, state->field);

    return 18 + len;
}

static char alpha_to_phone_digit(int ch)
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

static ssize_t get_display_phone_value(const struct input_state *state, itu_t_e164_t *e164, char *buffer, ssize_t size)
{
    itu_t_e164_t display_e164;
    char display_value[sizeof(e164->value)];
    int value_pos;
    int phone_pos;

    if((e164->context.input_mode == ITU_T_E164_INPUT_MODE_DIALING || e164->pos == 0) && state->phone_len > 0) {
        snprintf(buffer, size, "%s", state->phone);
        return strlen(buffer);
    }

    if(!state->accept_alphanumeric)
        return itu_t_e164_get_context_value(e164, buffer, size);

    display_e164 = *e164;
    snprintf(display_value, sizeof(display_value), "%s", e164->value);
    value_pos = strlen(display_value) - 1;

    for(phone_pos = state->phone_len - 1; phone_pos >= 0 && value_pos >= 0; phone_pos--) {
        unsigned char ch = (unsigned char)state->phone[phone_pos];

        if(isdigit(ch)) {
            value_pos--;
        } else if(isalpha(ch)) {
            char digit = alpha_to_phone_digit(ch);

            if(digit == display_value[value_pos])
                display_value[value_pos] = toupper(ch);
            value_pos--;
        }
    }

    snprintf(display_e164.value, sizeof(display_e164.value), "%s", display_value);
    return itu_t_e164_get_context_value(&display_e164, buffer, size);
}

static const char *component_int_value(char *buffer, size_t size, int value)
{
    if(value == 0) {
        buffer[0] = 0;
        return buffer;
    }

    snprintf(buffer, size, "%d", value);
    return buffer;
}

static void draw_form(WINDOW *win, const struct input_state *state, itu_t_e164_t *e164)
{
    const char *country;
    char buffer[BUFSIZ];
    char country_code[16];
    char area_code[16];
    char carrier_code[16];
    char national[BUFSIZ];
    char subscriber[BUFSIZ];
    int bytes;

    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Phone ");
    draw_field(win, 2, "DDI", field_value(state, FIELD_COUNTRY), state->field == FIELD_COUNTRY);
    draw_field(win, 3, "DDD", field_value(state, FIELD_AREA), state->field == FIELD_AREA);
    draw_field(win, 4, "Operadora", field_value(state, FIELD_CARRIER), state->field == FIELD_CARRIER);
    draw_restriction_field(win, 5, state);
    draw_mode_field(win, 6, state);
    draw_toggle_field(win, 7, "Alfanumerico", state->accept_alphanumeric, state->field == FIELD_ALPHANUMERIC);

    bytes = get_display_phone_value(state, e164, buffer, sizeof(buffer));
    draw_field(win, 9, "Numero", buffer, state->field == FIELD_PHONE);

    itu_t_e164_get_value(e164, buffer, sizeof(buffer));
    draw_field(win, 10, "Completo", e164->pos == 0 ? "" : buffer, 0);

    itu_t_e164_get_dialing_value(e164, buffer, sizeof(buffer));
    draw_field(win, 11, "Discagem", buffer, 0);

    component_int_value(country_code, sizeof(country_code), itu_t_e164_get_country_code(e164));
    component_int_value(area_code, sizeof(area_code), itu_t_e164_get_area_code(e164));
    component_int_value(carrier_code, sizeof(carrier_code), itu_t_e164_get_carrier_code(e164));
    itu_t_e164_get_national_value(e164, national, sizeof(national));
    itu_t_e164_get_subscriber_value(e164, subscriber, sizeof(subscriber));
    mvwprintw(win, 12, 2, "DDI: %-5s DDD: %-6s CSP: %-6s", country_code, area_code, carrier_code);
    mvwprintw(win, 13, 2, "Nacional: %-20s Assinante: %-18s", national, subscriber);

    country = known_country(e164);
    if(country != NULL)
        mvwprintw(win, 14, 2, "Pais: %-18s", country);
    else
        mvwprintw(win, 14, 2, "%-24s", "");

    mvwprintw(win, 14, 34, "Tipo: %-10s", itu_t_e164_number_kind_name(itu_t_e164_get_number_kind(e164)));
    mvwprintw(win, 15, 2, "Status: %-10s", e164_is_complete(e164) ? "completo" : "incompleto");
    mvwprintw(win, 16, 2, "TAB/baixo avancam. Shift-TAB/cima voltam. Enter conclui.");
    wmove(win, field_y(state->field), state->field == FIELD_PHONE ? 18 + bytes : field_cursor_x(state));
    wrefresh(win);
}

static void next_field(struct input_state *state)
{
    state->field = (state->field + 1) % FIELD_TOTAL;
}

static void previous_field(struct input_state *state)
{
    state->field = state->field == 0 ? FIELD_TOTAL - 1 : state->field - 1;
}

static int append_to_field(struct input_state *state, enum input_field field, int ch)
{
    char *buffer = field_buffer(state, field);
    size_t *len = field_len(state, field);
    size_t size = field_size(field);

    if(*len >= size - 1)
        return 0;

    buffer[*len] = ch;
    buffer[++(*len)] = 0;
    return 1;
}

static void trim_field(struct input_state *state, enum input_field field)
{
    char *buffer;
    size_t *len;

    if(field == FIELD_ALPHANUMERIC)
        return;

    buffer = field_buffer(state, field);
    len = field_len(state, field);

    if(*len == 0)
        return;

    buffer[--(*len)] = 0;
}

static void clear_phone(struct input_state *state)
{
    state->phone[0] = 0;
    state->phone_len = 0;
}

static int set_restriction(struct input_state *state, int ch)
{
    unsigned long previous;

    if(ch < '0' || ch > '2')
        return 0;

    previous = restriction_value(state);
    set_restriction_value(state, ch - '0');
    return previous != restriction_value(state);
}

static int handle_context_key(struct input_state *state, int ch)
{
    if(ch == KEY_BACKSPACE || ch == 127) {
        const char *buffer = field_value(state, state->field);
        size_t previous_len = strlen(buffer);

        trim_field(state, state->field);
        return strlen(buffer) != previous_len;
    }

    if(state->field == FIELD_RESTRICTION) {
        unsigned long previous = restriction_value(state);

        if(ch == KEY_LEFT) {
            cycle_restriction(state, -1);
            return previous != restriction_value(state);
        }
        if(ch == KEY_RIGHT || ch == ' ') {
            cycle_restriction(state, 1);
            return previous != restriction_value(state);
        }
        return set_restriction(state, ch);
    }

    if(state->field == FIELD_MODE) {
        unsigned long previous = mode_value(state);

        if(ch == KEY_LEFT || ch == '0' || ch == 'n' || ch == 'N') {
            set_mode_value(state, ITU_T_E164_INPUT_MODE_NUMBER);
        } else if(ch == KEY_RIGHT || ch == ' ') {
            cycle_mode(state);
        } else if(ch == '1' || ch == 'd' || ch == 'D') {
            set_mode_value(state, ITU_T_E164_INPUT_MODE_DIALING);
        }
        return previous != mode_value(state);
    }

    if(state->field == FIELD_ALPHANUMERIC) {
        int previous = state->accept_alphanumeric;

        if(ch == KEY_LEFT || ch == '0' || ch == 'n' || ch == 'N') {
            state->accept_alphanumeric = 0;
        } else if(ch == KEY_RIGHT || ch == ' ' || ch == '1' || ch == 's' || ch == 'S') {
            state->accept_alphanumeric = 1;
        }
        return previous != state->accept_alphanumeric;
    }

    if(ch >= 0 && ch <= 255 && isdigit((unsigned char)ch))
        return append_to_field(state, state->field, ch);

    return 0;
}

static void handle_phone_key(struct input_state *state, itu_t_e164_t *e164, int ch)
{
    char previous_value[sizeof(e164->value)];
    int appended_digit = 0;

    snprintf(previous_value, sizeof(previous_value), "%s", e164->value);

    if(ch == KEY_BACKSPACE || ch == 127) {
        trim_field(state, FIELD_PHONE);
    } else if((ch == '+' || ch == '(') && state->phone_len == 0 && prefix_allowed(e164, ch)) {
        append_to_field(state, FIELD_PHONE, ch);
    } else if(ch >= 0 && ch <= 255 && isdigit((unsigned char)ch)) {
        appended_digit = append_to_field(state, FIELD_PHONE, ch);
    } else if(state->accept_alphanumeric && ch >= 0 && ch <= 255 && isalpha((unsigned char)ch)) {
        appended_digit = append_to_field(state, FIELD_PHONE, ch);
    }

    itu_t_e164_set_value(e164, state->phone);
    if(appended_digit && strcmp(e164->value, previous_value) == 0 && state->phone_len > 0 &&
       (e164_is_complete(e164) || e164->pos >= 15)) {
        trim_field(state, FIELD_PHONE);
        itu_t_e164_set_value(e164, state->phone);
    }
}

void get_phone_number(WINDOW *win, itu_t_e164_t *e164) {
    int ch;
    struct input_state state;

    memset(&state, 0, sizeof(state));
    state.field = FIELD_PHONE;
    apply_context(e164, &state);
    draw_form(win, &state, e164);

    while(1) {
        ch = wgetch(win);

        if(ch == '\n' || ch == '\r') {
            break;
        } else if(ch == '\t' || ch == KEY_DOWN) {
            next_field(&state);
        } else if(ch == KEY_BTAB || ch == KEY_UP) {
            previous_field(&state);
        } else if(state.field == FIELD_PHONE) {
            handle_phone_key(&state, e164, ch);
        } else {
            if(handle_context_key(&state, ch))
                clear_phone(&state);
            apply_context(e164, &state);
        }
        draw_form(win, &state, e164);
    }
}

int main() {
    int startx, starty, width = INPUT_WIDTH, height = 18;
    WINDOW *win;

    itu_t_e164_t e164;

    load_numbering_plan();
    itu_t_e164_init(&e164);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    starty = (LINES - height) / 2; // Centraliza verticalmente
    startx = (COLS - width) / 2;  // Centraliza horizontalmente
    win = newwin(height, width, starty, startx);
    keypad(win, TRUE);

    get_phone_number(win, &e164);

    // Mostra o resultado final
    char buffer[BUFSIZ];
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    mvprintw(LINES - 2, 0, "Numero digitado: %s", buffer);
    refresh();

    getch();
    endwin();

    return 0;
}
