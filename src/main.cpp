#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <lua.hpp>

#include <thread>

#include <termios.h>
#include <unistd.h>

#include <ly/render/buffer.hpp>
#include <ly/render/lua_bindings.hpp>
#include <ly/render/utils.hpp>
#include <ly/render/widgets.hpp>
#include <ly/render/window.hpp>

int main(int argc, char* argv[]) {
    using namespace std::chrono;
    using namespace ly;

    constexpr auto tick_duration = 1ms;

    render::Window win;

    ly::render::lua::State state;
    auto widget = state.from_file("init.lua");

    float fps   = 0;
    size_t tick = 0;
    char cbuf   = 0;
    auto val    = render::lua::Value::float_val(10.);
    ly::render::set_raw_mode();
    ly::render::enter_alternate_screen();

    while (!state.should_exit()) {
        render::reset_cursor();
        auto t_start = high_resolution_clock::now();

        state.set_data("tick",
            render::lua::Value::integer((int64_t)tick));
        state.set_data("fps",
            render::lua::Value::float_val((double)fps));

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
        fps = 1000.0 / total_elapsed.count();
        tick++;
    }

    ly::render::unset_raw_mode();
    ly::render::leave_alternate_screen();
    return 0;
}
