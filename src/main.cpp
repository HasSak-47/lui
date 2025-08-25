#include <ly/render/widgets.hpp>
#include <ly/render/window.hpp>
#include <ly/ty.hpp>

#include <algorithm>
#include <chrono>
#include <thread>
#include "ly/render/utils.hpp"

bool BAD = false;

class TestElement : public ly::render::widgets::Element {
public:
    ~TestElement() override {}

    void content(ly::render::Buffer& buf) const override {
        BAD = true;
        buf.render_widget("test widget");
    };
    size_t content_width(
        const ly::render::Buffer& buf) const override {
        return buf.width() < 12 ? buf.width() : 12;
    };
    size_t content_height(
        const ly::render::Buffer& buf) const override {
        return std::max(
            (size_t)12 / buf.width(), (size_t)1);
    };
};

int main() {
    using namespace ly;
    render::Window win;
    render::widgets::RootElement root = {};
    root.add_child(
        std::shared_ptr<render::widgets::Element>(
            new TestElement{}));

    render::enter_alternate_screen();
    render::set_raw_mode();

    win.get_buf().render_widget(root);
    win.render();

    using namespace std::chrono;
    std::this_thread::sleep_for(2000ms);

    render::leave_alternate_screen();
    render::unset_raw_mode();
    printf("bad: %d", BAD);

    return 0;
}

/*
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

    constexpr auto tick_duration = 20ms;

    render::Window win;

    ly::render::lua::State state;
    state.set_function("set_color", [&](lua_State* L) {
        std::string type = lua_tostring(L, 1);

        render::ConsoleColor& color = (type == "fg")
                                          ? win.default_fc
                                          : win.default_bc;
        lua_getfield(L, 2, "type");
        lua_getfield(L, 2, "r");
        lua_getfield(L, 2, "g");
        lua_getfield(L, 2, "b");

        std::string ty = lua_tostring(L, -4);
        int r          = lua_tointeger(L, -3);
        int g          = lua_tointeger(L, -2);
        int b          = lua_tointeger(L, -1);
        lua_pop(L, 4);

        if (ty == "bit") {
            r     = (r & 1) << 0;
            g     = (g & 1) << 1;
            b     = (b & 1) << 2;
            color = render::ConsoleColor(r | g | b);
        }
        else if (ty == "8bit") {
            color =
                render::ConsoleColor(render::Color<ly::u8>(
                    r & 0xff, g & 0xff, b & 0xff));
        }

        return 0;
    });

    auto widget = state.from_file("init.lua");

    float fps   = 0;
    size_t tick = 0;
    char cbuf   = 0;
    auto val    = render::lua::Value::float_val(10.);
    ly::render::set_raw_mode();
    ly::render::enter_alternate_screen();

    win.init_buffer();
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
*/
