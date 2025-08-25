#ifndef __RENDER_BUFFER_HPP__
#define __RENDER_BUFFER_HPP__

#include <ly/ty.hpp>

#include <concepts>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

// std::ostream shenanigans
namespace ly::render {
class Buffer;
}

std::ostream& operator<<(
    std::ostream& other, const ly::render::Buffer& buf);

// definitions
namespace ly::render {

template <typename T>
struct Color {
    T r = {}, g = {}, b = {};
};

enum ColorBit : int8_t {
    BLACK  = 0b000,
    RED    = 0b001,
    GREEN  = 0b010,
    YELLOW = 0b011,
    BLUE   = 0b100,
    PURPLE = 0b101,
    CYAN   = 0b110,
    WHITE  = 0b111,
};

struct ConsoleColor {
private:
    enum class ColorType { Bit, TrueColor } _ty;
    union _ColorUnion {
        ColorBit bit_col;
        Color<u8> true_col;

        _ColorUnion(ColorBit col) { this->bit_col = col; }
        _ColorUnion(Color<u8> col) { this->true_col = col; }
    } dt;

public:
    ConsoleColor(Color<u8> col)
        : _ty(ColorType::TrueColor), dt(col) {};

    ConsoleColor(ColorBit col)
        : _ty(ColorType::Bit), dt(col) {};

    bool operator==(const ConsoleColor& other) const;
    void display_fc(bool override_default = false);
    void display_bc(bool override_default = false);

    static const ConsoleColor WHITE;
    static const ConsoleColor BLACK;
    static const ConsoleColor RED;
    static const ConsoleColor YELLOW;
    static const ConsoleColor GREEN;
    static const ConsoleColor PURPLE;
    static const ConsoleColor CYAN;
    static const ConsoleColor BLUE;
};

// char_t is a string bc I don't want to bother to
// conver it to utf8 when rendering in a console
using char_t = std::string;

struct Unit {
    char_t data = " ";
    ConsoleColor fc;
    ConsoleColor bc;
    Unit();
};

void render(Buffer& buf);

template <typename T>
concept OstreamFormattable =
    requires(std::ostream& os, const T& val) {
        { os << val } -> std::same_as<std::ostream&>;
    };

template <typename T>
concept Renderable =
    requires(ly::render::Buffer& buf, const T& widget) {
        { render(buf, widget) } -> std::same_as<void>;
    };

// utf8 representation of a string
class Su8 {
public:
    using iterator = std::vector<char_t>::iterator;
    using const_iterator =
        std::vector<char_t>::const_iterator;

private:
    std::vector<std::string> _cs;

public:
    Su8(const char* str);
    std::string to_string() const;

    std::string& operator[](size_t index);
    const std::string& operator[](size_t index) const;

    size_t size() const;
    iterator begin() { return _cs.begin(); }
    iterator end() { return _cs.end(); }
    const_iterator begin() const { return _cs.begin(); }
    const_iterator end() const { return _cs.end(); }
    const_iterator cbegin() const { return _cs.cbegin(); }
    const_iterator cend() const { return _cs.cend(); }
};

std::ostream& operator<<(std::ostream& other, const Su8& s);

class Buffer {
private:
    using _Buffer =
        std::shared_ptr<std::vector<std::vector<Unit>>>;
    _Buffer _data;
    size_t _x, _y;
    size_t _w, _h;

    Buffer(size_t x, size_t y, size_t w, size_t h,
        _Buffer buf);

public:
    Buffer(size_t w, size_t h,
        ConsoleColor fg = ConsoleColor::WHITE,
        ConsoleColor bg = ConsoleColor::BLACK);
    Buffer(const Buffer& other);
    Buffer(Buffer&& other);

    Buffer& operator=(const Buffer& other);
    Buffer& operator=(Buffer&& other);

    ~Buffer();

    Buffer get_sub_buffer(
        size_t x, size_t y, size_t w, size_t h);

    Unit& get(size_t x, size_t y);
    Unit& get(size_t x, size_t y) const;
    friend std::ostream& ::operator<<(
        std::ostream& other, const Buffer& buf);

    const size_t width() const;
    const size_t height() const;

    template <typename T>
        requires Renderable<T>
    void render_widget(const T& widget) {
        render(*this, widget);
    }

    template <typename T>
        requires(!Renderable<T> && OstreamFormattable<T>)
    void render_widget(const T& widget) {
        std::ostringstream ss;
        ss << widget;
        Su8 s      = ss.str().c_str();
        size_t idx = 0;
        for (const auto& c : s) {
            size_t i = idx % this->_w;
            size_t j = idx / this->_w;
            if (j >= this->_h) {
                break;
            }

            this->get(i, j).data = c;
            this->get(i, j).fc   = ConsoleColor::WHITE;
            idx++;
        }
    }
};

} // namespace ly::render

template <typename T>
inline bool operator==(const ly::render::Color<T>& a,
    const ly::render::Color<T>& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

template <typename T>
inline bool operator!=(const ly::render::Color<T>& a,
    const ly::render::Color<T>& b) {
    return !(a == b);
}

#endif
