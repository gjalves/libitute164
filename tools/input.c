#include <ncurses.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libitute164.h>

#define INPUT_WIDTH 40

void draw_input_box(WINDOW *win, int starty, int startx, const char *label) {
    mvwprintw(win, starty, startx, "%s", label);
    box(win, 0, 0);
    wrefresh(win);
}

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

static int starts_with(const char *value, const char *prefix)
{
    size_t len;

    if(prefix == NULL || prefix[0] == 0)
        return 0;

    len = strlen(prefix);
    return strncmp(value, prefix, len) == 0;
}

static int input_has_explicit_country(itu_t_e164_t *e164, const char *input)
{
    char country[8];
    const char *prefix;

    if(e164->context.country_code == 0)
        return 1;

    prefix = itu_t_e164_cc_2_international_prefix(e164->context.country_code);
    if(starts_with(input, prefix))
        return 1;

    snprintf(country, sizeof(country), "%lu", (unsigned long)e164->context.country_code);
    return starts_with(input, country);
}

static int input_has_explicit_area(itu_t_e164_t *e164, const char *input)
{
    char area[16];
    const char *prefix;

    if(e164->context.area_code == 0)
        return 1;

    snprintf(area, sizeof(area), "%lu", (unsigned long)e164->context.area_code);
    if(starts_with(input, area))
        return 1;

    prefix = itu_t_e164_cc_2_national_prefix(e164->context.country_code);
    if(starts_with(input, prefix))
        return starts_with(input + strlen(prefix), area);

    return 0;
}

static const char *display_phone_value(itu_t_e164_t *e164, const char *input, char *buffer, size_t size)
{
    const char *display = buffer;
    char prefix[24];

    itu_t_e164_get_value(e164, buffer, size);
    if(input[0] == 0 && e164->context.country_code != 0)
        return "";

    if(e164->context.country_code != 0 && !input_has_explicit_country(e164, input)) {
        snprintf(prefix, sizeof(prefix), "+%lu", (unsigned long)e164->context.country_code);
        if(starts_with(display, prefix)) {
            display += strlen(prefix);
            if(display[0] == ' ')
                display++;
        }
    }

    if(e164->context.area_code != 0 && !input_has_explicit_area(e164, input)) {
        snprintf(prefix, sizeof(prefix), "(%lu)", (unsigned long)e164->context.area_code);
        if(starts_with(display, prefix)) {
            display += strlen(prefix);
            if(display[0] == ' ')
                display++;
        }
    }

    return display;
}

static void draw_phone_state(WINDOW *win, int y, int x, itu_t_e164_t *e164, const char *input, int *bytes)
{
    const char *country;
    const char *display;
    char buffer[BUFSIZ];

    display = display_phone_value(e164, input, buffer, sizeof(buffer));
    *bytes = strlen(display);
    mvwprintw(win, y, x, "%-*s", INPUT_WIDTH - x - 2, display);

    country = known_country(e164);
    if(country != NULL)
        mvwprintw(win, y + 1, x, "Pais: %-*s", INPUT_WIDTH - x - 8, country);
    else
        mvwprintw(win, y + 1, x, "%-*s", INPUT_WIDTH - x - 2, "");
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

static void read_context_field(int y, const char *label, char *value, size_t size)
{
    mvprintw(y, 2, "%s: ", label);
    clrtoeol();
    echo();
    getnstr(value, size - 1);
    noecho();
}

static void configure_context(itu_t_e164_t *e164)
{
    char country[8];
    char area[8];
    itu_t_e164_context_t context;

    clear();
    mvprintw(1, 2, "Localidade padrao");
    mvprintw(2, 2, "Deixe em branco para nao usar.");
    read_context_field(4, "DDI", country, sizeof(country));

    memset(&context, 0, sizeof(context));
    context.country_code = parse_number(country);
    if(context.country_code != 0) {
        read_context_field(5, "DDD", area, sizeof(area));
        context.area_code = parse_number(area);
    }
    itu_t_e164_set_context(e164, &context);
    clear();
}

void get_phone_number(WINDOW *win, int y, int x, itu_t_e164_t *e164) {
    int ch;
    int bytes;
    char input[sizeof(e164->value)];
    size_t input_len = 0;

    input[0] = 0;

    draw_phone_state(win, y, x, e164, input, &bytes);
    wmove(win, y, x + bytes);
    wrefresh(win);

    //while (pos < max_len - 1) {
    while (1) {
        ch = wgetch(win);

        if (ch == '\n' || ch == '\r') {
            break; // Concluir com Enter
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            // Apagar o último caractere
            if(input_len > 0)
                input[--input_len] = 0;
        } else if (isdigit(ch)) {
            if(input_len < sizeof(input) - 1) {
                input[input_len++] = ch;
                input[input_len] = 0;
            }
        }
        itu_t_e164_set_value(e164, input);
        draw_phone_state(win, y, x, e164, input, &bytes);
        wmove(win, y, x + bytes);
        wrefresh(win);
    }
}

int main() {
    int startx, starty, width = INPUT_WIDTH, height = 7;
    WINDOW *win;

    itu_t_e164_t e164;

    load_numbering_plan();
    itu_t_e164_init(&e164);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    configure_context(&e164);

    starty = (LINES - height) / 2; // Centraliza verticalmente
    startx = (COLS - width) / 2;  // Centraliza horizontalmente
    win = newwin(height, width, starty, startx);

    draw_input_box(win, 1, 1, "Phone:");
    get_phone_number(win, 2, 2, &e164);

    // Mostra o resultado final
    char buffer[BUFSIZ];
    itu_t_e164_get_value(&e164, buffer, sizeof(buffer));
    mvprintw(LINES - 2, 0, "Numero digitado: %s", buffer);
    refresh();

    getch();
    endwin();

    return 0;
}
