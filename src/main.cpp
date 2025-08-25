#include <ly/render/buffer.hpp>
#include <ly/render/utils.hpp>
#include <ly/render/widgets.hpp>
#include <ly/render/window.hpp>
#include <ly/ty.hpp>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <memory>
#include <thread>

bool BAD = false;

using namespace ly::render;
using namespace ly::render::widgets;

class Progress : public Element {
public:
    float progress = 0.5;
    Progress() {}
    ~Progress() override {}

    void content(Buffer& buf) const override {
        buf.get(0, 0).data               = "[";
        buf.get(buf.width() - 1, 0).data = "]";
        for (size_t i = 0;
            i < (buf.width() - 2) * this->progress; ++i) {
            buf.get(1 + i, 0).data = "|";
        }
    }
    size_t content_width(const Buffer& buf) const override {
        return buf.width();
    }
    size_t content_height(
        const Buffer& buf) const override {
        return 1;
    }
};

class TestElement : public Element {
public:
    ~TestElement() override {}

    void content(Buffer& buf) const override {
        buf.render_widget("test widget");
    };
    size_t content_width(const Buffer& buf) const override {
        return buf.width() < 12 ? buf.width() : 12;
    };
    size_t content_height(
        const Buffer& buf) const override {
        return 1;
    };
};

int main() {
    using namespace ly;
    using namespace std::chrono;

    Window w;
    RootElement e;
    e.add_child(std::shared_ptr<Element>(new Progress{}));
    e.add_child(
        std::shared_ptr<Element>(new TestElement{}));

    enter_alternate_screen();

    w.get_buf().render_widget(e);
    w.render();

    std::this_thread::sleep_for(1000ms);
    leave_alternate_screen();

    return 0;
}
