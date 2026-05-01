#include <ncurses.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libitute164.h>

#define INPUT_WIDTH 72
#define PHONE_INPUT_SIZE 16

enum input_field {
    FIELD_COUNTRY,
    FIELD_AREA,
    FIELD_RESTRICTION,
    FIELD_ALPHANUMERIC,
    FIELD_PHONE,
    FIELD_TOTAL
};

struct input_state {
    char country[8];
    char area[8];
    char restriction[2];
    char phone[PHONE_INPUT_SIZE];
    size_t country_len;
    size_t area_len;
    size_t restriction_len;
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
    if(e164->cc.type == ITU_T_UNKNOWN || e164->cc.type == ITU_T_INCOMPLETE)
        return NULL;

    return itu_t_e164_cc_2_country(e164->cc.value);
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
        context.restriction = parse_number(state->restriction);
        if(context.restriction > ITU_T_E164_CONTEXT_RESTRICT_AREA)
            context.restriction = ITU_T_E164_CONTEXT_RESTRICT_NONE;
        if(context.restriction == ITU_T_E164_CONTEXT_RESTRICT_AREA && context.area_code == 0)
            context.restriction = ITU_T_E164_CONTEXT_RESTRICT_COUNTRY;
    }
    context.accept_alphanumeric = state->accept_alphanumeric;
    itu_t_e164_set_context(e164, &context);
    itu_t_e164_set_value(e164, state->phone);
}

static int prefix_allowed(const itu_t_e164_t *e164, int ch)
{
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
        case FIELD_RESTRICTION:
            return state->restriction;
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
        case FIELD_RESTRICTION:
            return &state->restriction_len;
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
        case FIELD_RESTRICTION:
            return state->restriction;
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
        case FIELD_RESTRICTION:
            return sizeof(((struct input_state *)0)->restriction);
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

static unsigned long restriction_value(const struct input_state *state)
{
    return parse_number(state->restriction);
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

static int field_cursor_x(const struct input_state *state)
{
    size_t len;

    if(state->field == FIELD_RESTRICTION)
        len = strlen(restriction_label(state->restriction));
    else if(state->field == FIELD_ALPHANUMERIC)
        len = strlen(field_value(state, state->field));
    else
        len = *field_len((struct input_state *)state, state->field);

    return 18 + len;
}

static void get_full_dialing_value(itu_t_e164_t *e164, char *buffer, size_t size)
{
    const char *prefix;

    if(size == 0)
        return;

    buffer[0] = 0;
    if(e164->pos == 0)
        return;

    if(e164->context.country_code != 0 && e164->cc.value == e164->context.country_code) {
        prefix = itu_t_e164_cc_2_national_prefix(e164->cc.value);
        if(prefix != NULL && prefix[0] != 0) {
            snprintf(buffer, size, "%s%s", prefix, &e164->value[e164->cc.len]);
            return;
        }
    }

    if(e164->context.country_code != 0) {
        prefix = itu_t_e164_cc_2_international_prefix(e164->context.country_code);
        if(prefix != NULL && prefix[0] != 0) {
            snprintf(buffer, size, "%s%s", prefix, e164->value);
            return;
        }
    }

    snprintf(buffer, size, "+%s", e164->value);
}

static void draw_form(WINDOW *win, const struct input_state *state, itu_t_e164_t *e164)
{
    const char *country;
    char buffer[BUFSIZ];
    int bytes;

    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Phone ");
    draw_field(win, 2, "DDI", field_value(state, FIELD_COUNTRY), state->field == FIELD_COUNTRY);
    draw_field(win, 3, "DDD", field_value(state, FIELD_AREA), state->field == FIELD_AREA);
    draw_restriction_field(win, 4, state);
    draw_toggle_field(win, 5, "Alfanumerico", state->accept_alphanumeric, state->field == FIELD_ALPHANUMERIC);

    bytes = itu_t_e164_get_context_value(e164, buffer, sizeof(buffer));
    draw_field(win, 7, "Numero", buffer, state->field == FIELD_PHONE);

    itu_t_e164_get_value(e164, buffer, sizeof(buffer));
    draw_field(win, 8, "Completo", e164->pos == 0 ? "" : buffer, 0);

    get_full_dialing_value(e164, buffer, sizeof(buffer));
    draw_field(win, 9, "Discagem", buffer, 0);

    country = known_country(e164);
    if(country != NULL)
        mvwprintw(win, 10, 2, "Pais: %-18s", country);
    else
        mvwprintw(win, 10, 2, "%-24s", "");

    mvwprintw(win, 10, 34, "Status: %-10s", e164_is_complete(e164) ? "completo" : "incompleto");
    mvwprintw(win, 11, 2, "TAB/baixo avancam. Shift-TAB/cima voltam. Enter conclui.");
    wmove(win, state->field == FIELD_PHONE ? 7 : state->field + 2, state->field == FIELD_PHONE ? 18 + bytes : field_cursor_x(state));
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

static void set_restriction(struct input_state *state, int ch)
{
    if(ch < '0' || ch > '2')
        return;

    set_restriction_value(state, ch - '0');
}

static void handle_context_key(struct input_state *state, int ch)
{
    if(ch == KEY_BACKSPACE || ch == 127) {
        trim_field(state, state->field);
        return;
    }

    if(state->field == FIELD_RESTRICTION) {
        if(ch == KEY_LEFT) {
            cycle_restriction(state, -1);
            return;
        }
        if(ch == KEY_RIGHT || ch == ' ') {
            cycle_restriction(state, 1);
            return;
        }
        set_restriction(state, ch);
        return;
    }

    if(state->field == FIELD_ALPHANUMERIC) {
        if(ch == KEY_LEFT || ch == '0' || ch == 'n' || ch == 'N')
            state->accept_alphanumeric = 0;
        else if(ch == KEY_RIGHT || ch == ' ' || ch == '1' || ch == 's' || ch == 'S')
            state->accept_alphanumeric = 1;
        return;
    }

    if(ch >= 0 && ch <= 255 && isdigit((unsigned char)ch))
        append_to_field(state, state->field, ch);
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
    if(appended_digit && strcmp(e164->value, previous_value) == 0 && state->phone_len > 0) {
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
            handle_context_key(&state, ch);
            apply_context(e164, &state);
        }
        draw_form(win, &state, e164);
    }
}

int main() {
    int startx, starty, width = INPUT_WIDTH, height = 14;
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
