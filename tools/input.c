#include <ncurses.h>
#include <ctype.h>
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

static void draw_phone_state(WINDOW *win, int y, int x, itu_t_e164_t *e164, int *bytes)
{
    const char *country;
    char buffer[BUFSIZ];

    *bytes = itu_t_e164_get_value(e164, buffer, sizeof(buffer));
    mvwprintw(win, y, x, "%-*s", INPUT_WIDTH - x - 2, buffer);

    country = known_country(e164);
    if(country != NULL)
        mvwprintw(win, y + 1, x, "Pais: %-*s", INPUT_WIDTH - x - 8, country);
    else
        mvwprintw(win, y + 1, x, "%-*s", INPUT_WIDTH - x - 2, "");
}

int phone_pos2mask(int pos)
{
    switch(pos) {
        case -1: return 1;
        case 0: return 2;
        case 1: return 3;
        case 2: return 6;
        case 3: return 7;
        case 4: return 10;
        case 5: return 11;
        case 6: return 12;
        case 7: return 13;
        case 8: return 14;
        case 9: return 16;
        case 10: return 17;
        case 11: return 18;
        case 12: return 19;
        case 13: return 20;
        default: return -1;
    }
}

void get_phone_number(WINDOW *win, int y, int x, itu_t_e164_t *e164) {
    int ch;
    int bytes;

    draw_phone_state(win, y, x, e164, &bytes);
    wmove(win, y, x + bytes);
    wrefresh(win);

    //while (pos < max_len - 1) {
    while (1) {
        ch = wgetch(win);

        if (ch == '\n' || ch == '\r') {
            break; // Concluir com Enter
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            // Apagar o último caractere
            itu_t_e164_del_digit(e164);
        } else if (isdigit(ch)) {
            itu_t_e164_add_digit(e164, ch);
        }
        draw_phone_state(win, y, x, e164, &bytes);
        wmove(win, y, x + bytes);
        wrefresh(win);
    }
}

int main() {
    int startx, starty, width = INPUT_WIDTH, height = 6;
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
