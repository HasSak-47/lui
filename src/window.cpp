#include <cstdio>
#include <sys/ioctl.h>
#include <unistd.h>

#include <ly/render/buffer.hpp>
#include <ly/render/window.hpp>

using namespace ly::render;

static Buffer _gen_buffer() {
    struct winsize w;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    return Buffer(w.ws_col, w.ws_row);
}

Window::Window()
    : _front(_gen_buffer()), _back(_gen_buffer()) {
    struct winsize w;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    _width  = w.ws_col;
    _height = w.ws_row;
}

Window::~Window() {}

void Window::resize() {
    struct winsize w;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    if (_height != w.ws_row || _width != w.ws_col) {
        _front  = Buffer(w.ws_col, w.ws_row);
        _back   = Buffer(w.ws_col, w.ws_row);
        _width  = w.ws_col;
        _height = w.ws_row;
    }
}

void Window::render() {
    ConsoleColor last = ConsoleColor::WHITE;
    last.display();

    for (size_t y = 0; y < _back.height(); ++y) {
        for (size_t x = 0; x < _back.width(); ++x) {
            auto& cur = _back.get(x, y);
            // update color if needed
            if (cur.col != last) {
                last = cur.col;
                last.display();
            }

            printf("%lc", cur.chr);
            cur.col = ConsoleColor::WHITE;
        }
    }

    last = ConsoleColor::WHITE;
    last.display();
}

Buffer Window::get_subbuf(
    size_t x, size_t y, size_t w, size_t h) {
    return _back.get_sub_buffer(x, y, w, h);
}
