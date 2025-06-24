#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <lua.hpp>

#include <thread>

#include <termios.h>
#include <unistd.h>

#include <ly/render/buffer.hpp>
#include <ly/render/lua_bindings.hpp>
#include <ly/render/widgets.hpp>
#include <ly/render/window.hpp>

bool got_original = false;

struct termios orig_termios = {};

void unset_raw_mode() {
    if (got_original)
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void set_raw_mode() {
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

int main(int argc, char* argv[]) {
    using namespace std::chrono;
    using namespace ly;

    constexpr auto tick_duration = 16.6666ms;
    render::Window win;

    ly::render::lua::State state;
    auto widget = state.from_file("init.lua");

    size_t tick = 0;
    char cbuf   = 0;
    auto val    = render::lua::Value::float_val(10.);

    printf("\e[?1049h"); // enter alternate screen
    printf("\e[0;0H");   // move cursor to 0
    printf("\e[?25l");   // hide cursor
    fflush(stdout);
    set_raw_mode();

    while (!state.should_exit()) {
        auto t_start = high_resolution_clock::now();

        printf("\e[0;0H"); // return cursor to 0,0
        fflush(stdout);

        if (read(STDIN_FILENO, &cbuf, 1) > 0) {
            state.press(cbuf);
        }

        widget.update();
        win.get_buf().render_widget(widget);
        win.render();

        // tick rate calc
        auto t_now = high_resolution_clock::now();
        auto delta = t_now - t_start;

        // wait for end of tick
        if (delta < tick_duration)
            std::this_thread::sleep_for(
                tick_duration - delta);

        // Optional: print elapsed time for debugging
        auto t_end = high_resolution_clock::now();
        auto total_elapsed =
            duration_cast<milliseconds>(t_end - t_start);
        float fps = 1000.0 / total_elapsed.count();
        tick++;
        state.set_data("tick",
            render::lua::Value::integer((int64_t)tick));
        state.set_data("fps",
            render::lua::Value::float_val((double)fps));
    }

    unset_raw_mode();
    printf("\e[?1049l"); // leave alternate screen
    printf("\e[?25h");   // show cursor
    fflush(stdout);
    return 0;
}
