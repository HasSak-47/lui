#ifndef __RENDER_WINDOW_HPP__
#define __RENDER_WINDOW_HPP__

#include <ly/int.hpp>
#include <ly/render/buffer.hpp>

namespace ly::render {

class Window {
    Buffer _front;
    // TODO: there are possible gains for using the back to
    // check if there character should be there or not, and
    // just not render it and just render the differences
    Buffer _back;
    size_t _width, _height;

public:
    Window();
    ~Window();

    Buffer get_subbuf(
        size_t x, size_t y, size_t w, size_t h);
    Buffer& get_buf() { return _back; }

    void render();
    void resize();

    const size_t width() const { return _width; }
    const size_t height() const { return _height; }
};

} // namespace ly::render

#endif
