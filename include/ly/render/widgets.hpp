#ifndef __RENDER_WIDGETS_HPP__
#define __RENDER_WIDGETS_HPP__

#include <ly/render/buffer.hpp>
#include <memory>

namespace ly::render::widgets {

class Widget {
public:
    virtual ~Widget() {};
    virtual void update() {};
    virtual void bind(std::shared_ptr<Widget> w) {}
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
