#ifndef __RENDER_WIDGETS_HPP__
#define __RENDER_WIDGETS_HPP__

#include <algorithm>
#include <ly/render/buffer.hpp>
#include <ly/state.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ly::render::widgets {

template <typename T>
struct Vec2 {
    T x, y;
};

template <typename T>
struct Vec4 {
    T a, b, c, d;
};

template struct Vec2<size_t>;

class Widget {
public:
    ~Widget() {}
    virtual void render(Buffer& buf) const = 0;
};

inline void render(Buffer& buf, const Widget& w) {
    w.render(buf);
}

class Element : public Widget {
public:
    struct Unit {
        enum class Type {
            Absolute,
            Relative,
        } ty = Type::Absolute;
        union {
            size_t abs = {};
            float rel;
        };

        Unit to_abs(size_t dim) const {
            if (this->ty == Type::Absolute)
                return *this;

            Unit a = {};
            a.abs  = dim * this->rel;
            return a;
        }
    };

    using Attr_t =
        std::unordered_map<std::string, ly::Value>;

    using Child_t = std::shared_ptr<Element>;

public:
    std::vector<std::string> _class = {};

    std::string _id = {};
    Attr_t _attrs   = {};

    std::vector<Child_t> _childs = {};
    Element* _parent             = nullptr;

public:
    Element() {}
    Element(std::string id) { this->_id = id; }
    virtual ~Element() {}

    bool transparent = false;
    /**
     * a = left
     * b = top
     * c = right
     * d = bottom
     */
    Vec4<Unit> margin  = {};
    Vec4<Unit> padding = {};
    Unit width = {}, height = {};

    std::optional<Child_t> find_id(std::string id);

    std::vector<Child_t>& get_childs() { return _childs; };

    void add_child(Child_t element);

    virtual void content(Buffer& buf) const = 0;
    virtual size_t content_width(
        const Buffer& buf) const = 0;
    virtual size_t content_height(
        const Buffer& buf) const = 0;

    void render(Buffer& buf) const override final;
};

class RootElement : public Element {
public:
    ~RootElement() override {}

    void content(Buffer& buf) const override {};
    size_t content_width(const Buffer& buf) const override {
        return 0;
    };
    size_t content_height(
        const Buffer& buf) const override {
        return 0;
    };
};

} // namespace ly::render::widgets

#endif
