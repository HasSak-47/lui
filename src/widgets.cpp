#include <cstdio>
#include <ly/render/widgets.hpp>

using namespace ly::render;
using namespace ly::render::widgets;

std::optional<Element::Child_t> Element::find_id(
    std::string id) {
    auto& childs = this->_childs;
    auto val = std::find_if(childs.begin(), childs.end(),
        [&](auto c) { return c->_id == id; });

    if (val == childs.end()) {
        for (auto& child : childs) {
            auto found = child->find_id(id);
            if (found)
                return found;
        }
        return {};
    }

    return *val;
};

void Element::add_child(Element::Child_t element) {
    element->_parent = this;
    this->_childs.push_back(element);
};

void Element::render(Buffer& buf) const {
    auto h = buf.height();
    auto w = buf.width();

    Vec4<Unit> padding = {
        this->padding.a.to_abs(w),
        this->padding.b.to_abs(h),
        this->padding.c.to_abs(w),
        this->padding.d.to_abs(h),
    };

    if (padding.a.abs + padding.c.abs >= w)
        return;

    if (padding.b.abs + padding.d.abs >= h)
        return;

    size_t wp    = w - padding.c.abs;
    size_t hp    = h - padding.d.abs;
    auto usefull = buf.get_sub_buffer(
        padding.a.abs, padding.b.abs, wp, hp);

    size_t cw = this->content_width(usefull);
    size_t ch = this->content_height(usefull);

    auto content_buffer =
        usefull.get_sub_buffer(0, 0, cw, ch);

    this->content(content_buffer);

    size_t offset = ch;
    for (size_t i = 0; i < this->_childs.size(); ++i) {
        if (offset > hp)
            return;

        auto& child = this->_childs[i];

        Vec4<Unit> margin = {
            child->margin.a.to_abs(w),
            child->margin.b.to_abs(h),
            child->margin.c.to_abs(w),
            child->margin.d.to_abs(h),
        };

        if (margin.a.abs + margin.c.abs >= w)
            return;

        if (margin.b.abs + margin.d.abs >= h)
            return;

        size_t lwp = wp - padding.c.abs;
        size_t lhp = (hp - offset) - padding.d.abs;
        auto sb    = usefull.get_sub_buffer(
            margin.a.abs, offset + margin.b.abs, lwp, lhp);

        sb.render_widget(*child);
    }
}
