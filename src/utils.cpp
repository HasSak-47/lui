#include <ly/render/utils.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

using namespace ly;

static bool got_original = false;

static struct termios orig_termios = {};

void render::unset_raw_mode() {
    if (got_original)
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void render::set_raw_mode() {
    if (!got_original) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(unset_raw_mode);
        got_original = true;
    }

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void render::reset_cursor() {
    printf("\e[0;0H"); // return cursor to 0,0
    fflush(stdout);
}
void render::enter_alternate_screen() {
    printf("\e[?1049h"); // enter alternate screen
    printf("\e[0;0H");   // move cursor to 0
    printf("\e[?25l");   // hide cursor
    fflush(stdout);
}
void render::leave_alternate_screen() {
    printf("\e[?1049l"); // leave alternate screen
    printf("\e[?25h");   // show cursor
    fflush(stdout);
}
