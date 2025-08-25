#include <cstdio>
#include <ly/render/widgets.hpp>
#include <ly/render/window.hpp>
#include <ly/ty.hpp>

#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
#include "ly/render/buffer.hpp"
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
    using namespace std::chrono;

    render::Window w;
    render::widgets::RootElement e;
    e.add_child(std::shared_ptr<render::widgets::Element>(
        new TestElement{}));

    render::enter_alternate_screen();

    w.get_buf().render_widget(e);
    w.render();
    std::this_thread::sleep_for(1000ms);

    render::leave_alternate_screen();
    printf("bad: %d", BAD);
    return 0;
}
