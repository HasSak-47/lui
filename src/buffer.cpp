#include <memory>
#include <ostream>
#include <utility>
#include <vector>

#include <ly/render/buffer.hpp>

namespace ly::render {
const ConsoleColor ConsoleColor::BLACK =
    ConsoleColor(ColorBit::BLACK);
const ConsoleColor ConsoleColor::RED =
    ConsoleColor(ColorBit::RED);
const ConsoleColor ConsoleColor::YELLOW =
    ConsoleColor(ColorBit::YELLOW);
const ConsoleColor ConsoleColor::GREEN =
    ConsoleColor(ColorBit::GREEN);
const ConsoleColor ConsoleColor::PURPLE =
    ConsoleColor(ColorBit::PURPLE);
const ConsoleColor ConsoleColor::CYAN =
    ConsoleColor(ColorBit::CYAN);
const ConsoleColor ConsoleColor::BLUE =
    ConsoleColor(ColorBit::BLUE);
const ConsoleColor ConsoleColor::WHITE =
    ConsoleColor(ColorBit::WHITE);
} // namespace ly::render

using namespace ly::render;

bool ConsoleColor::operator==(
    const ConsoleColor& other) const {
    if (this->_ty != other._ty) {
        return false;
    }

    if (this->_ty == ConsoleColor::ColorType::Bit) {
        return this->dt.bit_col == other.dt.bit_col;
    }
    return this->dt.true_col == other.dt.true_col;
}

// TODO: override defaults
void ConsoleColor::display_bc(bool override_default) {
    switch (this->_ty) {
    case ConsoleColor::ColorType::Bit:
        printf("\e[4%dm", (int)this->dt.bit_col);
        break;
    case ConsoleColor::ColorType::TrueColor:
        printf("\e[48;2;%d;%d;%dm", this->dt.true_col.r,
            this->dt.true_col.g, this->dt.true_col.b);
        break;
    }
}

void ConsoleColor::display_fc(bool override_default) {
    switch (this->_ty) {
    case ConsoleColor::ColorType::Bit:
        printf("\e[3%dm", (int)this->dt.bit_col);
        break;
    case ConsoleColor::ColorType::TrueColor:
        printf("\e[38;2;%d;%d;%dm", this->dt.true_col.r,
            this->dt.true_col.g, this->dt.true_col.b);
        break;
    }
}

Unit::Unit()
    : fc(ConsoleColor::WHITE), bc(ConsoleColor::BLACK) {}

static size_t utf8_char_length(unsigned char c) {
    if ((c & 0b10000000) == 0)
        return 1;
    if ((c & 0b11100000) == 0b11000000)
        return 2;
    if ((c & 0b11110000) == 0b11100000)
        return 3;
    if ((c & 0b11111000) == 0b11110000)
        return 4;
    return 1; // fallback, invalid UTF-8
}

Su8::Su8(const char* str) {
    const char* p = str;
    while (*p) {
        size_t len = utf8_char_length(
            static_cast<unsigned char>(*p));
        _cs.emplace_back(p, len);
        p += len;
    }
}

std::string Su8::to_string() const {
    std::string result;
    for (const auto& c : _cs) result += c;
    return result;
}

std::string& Su8::operator[](size_t index) {
    if (index >= _cs.size())
        throw std::out_of_range("Su8 index out of range");
    return _cs[index];
}

const std::string& Su8::operator[](size_t index) const {
    if (index >= _cs.size())
        throw std::out_of_range("Su8 index out of range");
    return _cs[index];
}

size_t Su8::size() const {
    return _cs.size();
}

Buffer::~Buffer() {}

Buffer::Buffer(
    size_t x, size_t y, size_t w, size_t h, _Buffer buf)
    : _x(x), _y(y), _w(w), _h(h) {

    this->_data = buf;
}

Buffer::Buffer(
    size_t w, size_t h, ConsoleColor fg, ConsoleColor bg)
    : _x(0), _y(0), _w(w), _h(h) {

    _Buffer buf =
        std::make_shared<std::vector<std::vector<Unit>>>();
    buf->resize(w);

    for (size_t i = 0; i < w; ++i) {
        std::vector<Unit> col = {};

        for (size_t j = 0; j < h; ++j) {
            Unit u;
            u.fc = fg;
            u.bc = bg;
            col.push_back(u);
        }

        (*buf)[i] = col;
    }
    this->_data = buf;
}

Buffer::Buffer(const Buffer& other)
    : _x(other._x), _y(other._y), _w(other._w),
      _h(other._h) {
    this->_data = other._data;
}

Buffer::Buffer(Buffer&& other)
    : _x(other._x), _y(other._y), _w(other._w),
      _h(other._h) {
    this->_data = std::move(other._data);
}

Buffer& Buffer::operator=(const Buffer& other) {
    this->_data = other._data;
    this->_x    = other._x;
    this->_y    = other._y;
    this->_h    = other._h;
    this->_w    = other._w;

    return *this;
}

Buffer& Buffer::operator=(Buffer&& other) {
    this->_data = std::move(other._data);
    this->_x    = other._x;
    this->_y    = other._y;
    this->_h    = other._h;
    this->_w    = other._w;
    return *this;
}

Buffer Buffer::get_sub_buffer(
    size_t x, size_t y, size_t w, size_t h) {

    return Buffer(
        this->_x + x, this->_y + y, w, h, this->_data);
}

Unit& Buffer::get(size_t x, size_t y) const {
    x += this->_x;
    y += this->_y;

    if (x >= (*_data).size())
        x = (*_data).size() - 1;
    if (y >= (*_data)[x].size())
        y = (*_data)[x].size() - 1;

    return (*this->_data)[x][y];
}

Unit& Buffer::get(size_t x, size_t y) {
    x += this->_x;
    y += this->_y;

    if (x >= (*_data).size())
        x = (*_data).size() - 1;
    if (y >= (*_data)[x].size())
        y = (*_data)[x].size() - 1;

    return (*this->_data)[x][y];
}

std::ostream& operator<<(
    std::ostream& os, const Buffer& buf) {
    for (size_t j = 0; j < buf._h; ++j) {
        for (size_t i = 0; i < buf._w; ++i) {
            os << buf.get(i, j).data;
        }
        os << '\n';
    }
    return os;
}

const size_t Buffer::width() const {
    return this->_w;
}
const size_t Buffer::height() const {
    return this->_h;
};
