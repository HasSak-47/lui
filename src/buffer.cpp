#include <functional>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

#include <ly/render/buffer.hpp>

namespace ly::render {
const ConsoleColor ConsoleColor::WHITE =
    ConsoleColor(ConsoleColor::_ColorUnion::_BitCol::WHITE);
const ConsoleColor ConsoleColor::RED =
    ConsoleColor(ConsoleColor::_ColorUnion::_BitCol::RED);
const ConsoleColor ConsoleColor::YELLOW = ConsoleColor(
    ConsoleColor::_ColorUnion::_BitCol::YELLOW);
const ConsoleColor ConsoleColor::GREEN =
    ConsoleColor(ConsoleColor::_ColorUnion::_BitCol::GREEN);
const ConsoleColor ConsoleColor::PURPLE = ConsoleColor(
    ConsoleColor::_ColorUnion::_BitCol::PURPLE);
const ConsoleColor ConsoleColor::CYAN =
    ConsoleColor(ConsoleColor::_ColorUnion::_BitCol::CYAN);
const ConsoleColor ConsoleColor::BLUE =
    ConsoleColor(ConsoleColor::_ColorUnion::_BitCol::BLUE);
} // namespace ly::render

using namespace ly::render;

bool ConsoleColor::operator==(
    const ConsoleColor& other) const {
    if (this->_ty != other._ty) {
        return false;
    }

    if (this->_ty == ConsoleColor::Bit) {
        return this->dt.bit_col == other.dt.bit_col;
    }
    return this->dt.true_col == other.dt.true_col;
}

void ConsoleColor::display() {
    switch (this->_ty) {
    case Bit:
        printf("\e[3%dm", (int)this->dt.bit_col);
        break;
    case TrueColor:
        printf("\e[38;2;%d;%d;%dm", this->dt.true_col.r,
            this->dt.true_col.g, this->dt.true_col.b);
        break;
    }
}

Unit::Unit() : col(ConsoleColor::WHITE) {}

Buffer::~Buffer() {}

Buffer::Buffer(
    size_t x, size_t y, size_t w, size_t h, _Buffer buf)
    : _x(x), _y(y), _w(w), _h(h) {

    this->_data = buf;
}

Buffer::Buffer(size_t w, size_t h)
    : _x(0), _y(0), _w(w), _h(h) {

    _Buffer buf =
        std::make_shared<std::vector<std::vector<Unit>>>();
    buf->resize(w);

    for (size_t i = 0; i < w; ++i) {
        std::vector<Unit> col = {};

        for (size_t j = 0; j < h; ++j)
            col.push_back(Unit());

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
            os << (char)buf.get(i, j).chr;
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
