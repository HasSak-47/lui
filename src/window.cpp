#include <cstdio>
#include <print>
#include <sys/ioctl.h>
#include <unistd.h>

#include <ly/render/buffer.hpp>
#include <ly/render/window.hpp>
#include "ly/render/utils.hpp"

using namespace ly::render;

Window::Window() : _front(10, 10), _back(10, 10) {}

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

void Window::init_buffer() {
    struct winsize w;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    _width       = w.ws_col;
    _height      = w.ws_row;
    this->_front = Buffer(_width, _height, this->default_fc,
        this->default_bc);
    this->_back  = Buffer(_width, _height, this->default_fc,
         this->default_bc);
}

void Window::render() {
    ConsoleColor last_fc = this->default_fc;
    ConsoleColor last_bc = this->default_bc;
    last_fc.display_fc();
    last_bc.display_bc();

    reset_cursor();
    for (size_t y = 0; y < _back.height(); ++y) {
        for (size_t x = 0; x < _back.width(); ++x) {
            auto& cur = _back.get(x, y);

            if (cur.fc != last_fc) {
                cur.fc.display_fc();
                last_fc = cur.fc;
            }

            if (cur.bc != last_bc) {
                cur.bc.display_bc();
                last_bc = cur.bc;
            }

            // this is a bottleneck according to perf lol
            // maybe I should just create a long strign and
            // print that idk
            printf("%s", cur.data.c_str());

            cur.fc.display_fc();
            cur.bc.display_bc();
            cur.fc   = this->default_fc;
            cur.bc   = this->default_bc;
            cur.data = " ";
        }
    }
}

Buffer Window::get_subbuf(
    size_t x, size_t y, size_t w, size_t h) {
    return _back.get_sub_buffer(x, y, w, h);
}
