#ifndef __RENDER_WIDGETS_HPP__
#define __RENDER_WIDGETS_HPP__

#include <ly/render/buffer.hpp>
#include <memory>

namespace ly::render::widgets {

class Widget;
class RootWidget;

struct Vec2 {
    size_t x, y;
};

class Widget {
protected:
    struct Bind {
        std::shared_ptr<Widget> W;
        Vec2 pos;
        Vec2 dim;
    };

    std::vector<Bind> _binds;
    void _update();

public:
    virtual ~Widget() {};
    virtual void update() {};
    virtual void bind(std::shared_ptr<Widget> W, size_t x,
        size_t y, size_t w, size_t h) {}
    virtual void render(Buffer& buffer) const = 0;
};

} // namespace ly::render::widgets

namespace ly::render {
inline void render(
    Buffer& buf, const widgets::Widget& widget) {
    widget.render(buf);
}
} // namespace ly::render

namespace ly::render::widgets {
template <typename T>
concept Encapsulable =
    Renderable<T> || OstreamFormattable<T> ||
    std::derived_from<T, Widget>;

} // namespace ly::render::widgets

#endif
