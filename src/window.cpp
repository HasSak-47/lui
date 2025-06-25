#include <cstdio>
#include <print>
#include <sys/ioctl.h>
#include <unistd.h>

#include <ly/render/buffer.hpp>
#include <ly/render/window.hpp>
#include "ly/render/utils.hpp"

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
    ConsoleColor last_fc = ConsoleColor::WHITE;
    ConsoleColor last_bc = ConsoleColor::BLACK;
    last_fc.display_fc();
    last_bc.display_bc();

    reset_cursor();
    for (size_t y = 0; y < _back.height(); ++y) {
        for (size_t x = 0; x < _back.width(); ++x) {
            auto& cur = _back.get(x, y);

            cur.fc.display_fc();
            cur.bc.display_bc();

            printf("%s", cur.data.c_str());

            cur.fc.display_fc();
            cur.bc.display_bc();
            cur.fc   = ConsoleColor::WHITE;
            cur.bc   = ConsoleColor::BLACK;
            cur.data = " ";
        }
    }
}

Buffer Window::get_subbuf(
    size_t x, size_t y, size_t w, size_t h) {
    return _back.get_sub_buffer(x, y, w, h);
}
