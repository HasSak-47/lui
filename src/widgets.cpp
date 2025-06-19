#include <algorithm>
#include <iomanip>
#include <ly/render/widgets.hpp>
#include <sstream>

using namespace ly::render;
using namespace ly::render::widgets;

lua::Value::Value()
    : type_(Ty::None), data_(std::monostate{}) {}

lua::Value::Value(int64_t val)
    : type_(Ty::Integer), data_(val) {}

Value::Value(double val) : type_(Ty::Float), data_(val) {}

Value::Value(const std::string& val)
    : type_(Ty::String), data_(val) {}

Value::Value(std::string&& val)
    : type_(Ty::String), data_(std::move(val)) {}

Value::Value(const MapType& val)
    : type_(Ty::Map), data_(val) {}

Value::Value(MapType&& val)
    : type_(Ty::Map), data_(std::move(val)) {}

Value::Value(const ArrayType& val)
    : type_(Ty::Array), data_(val) {}

Value::Value(ArrayType&& val)
    : type_(Ty::Array), data_(std::move(val)) {}

Value Value::none() {
    return Value();
}

Value Value::integer(int64_t val) {
    return Value(val);
}

Value Value::integer(int val) {
    return Value(static_cast<int64_t>(val));
}

Value Value::float_val(double val) {
    return Value(val);
}

Value Value::float_val(float val) {
    return Value(static_cast<double>(val));
}

Value Value::string(const std::string& val) {
    return Value(val);
}

Value Value::string(std::string&& val) {
    return Value(std::move(val));
}

Value Value::string(const char* val) {
    return Value(std::string(val));
}

Value Value::map() {
    return Value(MapType{});
}

Value Value::map(const MapType& val) {
    return Value(val);
}

Value Value::map(MapType&& val) {
    return Value(std::move(val));
}

Value Value::array() {
    return Value(ArrayType{});
}

Value Value::array(const ArrayType& val) {
    return Value(val);
}

Value Value::array(ArrayType&& val) {
    return Value(std::move(val));
}

Value::Ty Value::type() const {
    return type_;
}

bool Value::is_none() const {
    return type_ == Ty::None;
}

bool Value::is_integer() const {
    return type_ == Ty::Integer;
}

bool Value::is_float() const {
    return type_ == Ty::Float;
}

bool Value::is_string() const {
    return type_ == Ty::String;
}

bool Value::is_map() const {
    return type_ == Ty::Map;
}

bool Value::is_array() const {
    return type_ == Ty::Array;
}

int64_t Value::as_integer() const {
    if (type_ != Ty::Integer) {
        throw std::runtime_error("Value is not an integer");
    }
    return std::get<int64_t>(data_);
}

double Value::as_float() const {
    if (type_ != Ty::Float) {
        throw std::runtime_error("Value is not a float");
    }
    return std::get<double>(data_);
}

const std::string& Value::as_string() const {
    if (type_ != Ty::String) {
        throw std::runtime_error("Value is not a string");
    }
    return std::get<std::string>(data_);
}

const Value::MapType& Value::as_map() const {
    if (type_ != Ty::Map) {
        throw std::runtime_error("Value is not a map");
    }
    return std::get<MapType>(data_);
}

Value::MapType& Value::as_map() {
    if (type_ != Ty::Map) {
        throw std::runtime_error("Value is not a map");
    }
    return std::get<MapType>(data_);
}

const Value::ArrayType& Value::as_array() const {
    if (type_ != Ty::Array) {
        throw std::runtime_error("Value is not an array");
    }
    return std::get<ArrayType>(data_);
}

Value::ArrayType& Value::as_array() {
    if (type_ != Ty::Array) {
        throw std::runtime_error("Value is not an array");
    }
    return std::get<ArrayType>(data_);
}

Value& Value::operator[](const std::string& key) {
    if (type_ != Ty::Map) {
        throw std::runtime_error("Value is not a map");
    }
    return std::get<MapType>(data_)[key];
}

const Value& Value::operator[](
    const std::string& key) const {
    if (type_ != Ty::Map) {
        throw std::runtime_error("Value is not a map");
    }
    return std::get<MapType>(data_).at(key);
}

Value& Value::operator[](size_t index) {
    if (type_ != Ty::Array) {
        throw std::runtime_error("Value is not an array");
    }
    return std::get<ArrayType>(data_)[index];
}

const Value& Value::operator[](size_t index) const {
    if (type_ != Ty::Array) {
        throw std::runtime_error("Value is not an array");
    }
    return std::get<ArrayType>(data_)[index];
}

size_t Value::size() const {
    switch (type_) {
    case Ty::Map:
        return std::get<MapType>(data_).size();
    case Ty::Array:
        return std::get<ArrayType>(data_).size();
    case Ty::String:
        return std::get<std::string>(data_).size();
    default:
        throw std::runtime_error(
            "Value type does not support size()");
    }
}

bool Value::empty() const {
    switch (type_) {
    case Ty::None:
        return true;
    case Ty::Map:
        return std::get<MapType>(data_).empty();
    case Ty::Array:
        return std::get<ArrayType>(data_).empty();
    case Ty::String:
        return std::get<std::string>(data_).empty();
    default:
        return false;
    }
}

bool Value::operator==(const Value& other) const {
    if (type_ != other.type_)
        return false;
    return data_ == other.data_;
}

bool Value::operator!=(const Value& other) const {
    return !(*this == other);
}

void Block::render(Buffer& buf, size_t tick) const {
    if (buf.width() < 2 || buf.height() < 2) {
        return;
    }
    size_t w = buf.width() - 1;
    size_t h = buf.height() - 1;

    for (size_t i = 1; i < w; ++i) {
        buf.get(i, 0).chr = '-';
        buf.get(i, h).chr = '-';
    }

    for (size_t j = 1; j < h; ++j) {
        buf.get(0, j).chr = '|';
        buf.get(w, j).chr = '|';
    }

    buf.get(0, 0).chr = '+';
    buf.get(0, h).chr = '+';
    buf.get(w, 0).chr = '+';
    buf.get(w, h).chr = '+';
}

void Text::render(Buffer& buf, size_t tick) const {
    size_t max = std::min(buf.width(), this->_text.size());
    for (size_t i = 0; i < max; ++i) {
        buf.get(i, 0).chr = this->_text[i];
        buf.get(i, 0).col = this->_col;
    }
}

void Bar::render(Buffer& buf, size_t tick) const {
    size_t end = buf.width();
    if (this->_show && buf.width() > 7) {
        constexpr size_t sub_width = 10;
        size_t x = buf.width() - sub_width;

        auto sub = buf.get_sub_buffer(x, 0, sub_width, 1);

        // Format the percentage string
        std::ostringstream ss_raw;
        ss_raw << std::fixed << std::setprecision(1)
               << this->_val * 100 << '%';
        std::string str = ss_raw.str();

        // Truncate from the left if string is too long
        if (str.length() > sub_width)
            str = str.substr(str.length() - sub_width);

        // Right-align the string
        std::ostringstream ss;
        ss << std::setw(sub_width) << str;

        sub.render_widget(ss.str());
        end -= sub_width;
    }

    buf.get(0, 0).chr   = '[';
    buf.get(end, 0).chr = ']';

    size_t len = (end - 1);
    size_t w   = len * std::min(1.0f, this->_val);

    for (size_t i = 0; i < w; ++i) {
        buf.get(1 + i, 0).chr = '|';
    }
}

void MultiBar::render(Buffer& buf, size_t tick) const {
    size_t end = buf.width();

    if (this->_show && buf.width() > 7) {
        constexpr size_t sub_width = 10;
        size_t x = buf.width() - sub_width;

        auto sub  = buf.get_sub_buffer(x, 0, sub_width, 1);
        float val = 0.;
        for (const auto& v : this->_val) {
            val += v;
        }

        // Format the percentage string
        std::ostringstream ss_raw;
        ss_raw << std::fixed << std::setprecision(1)
               << val * 100 << '%';
        std::string str = ss_raw.str();

        // Truncate from the left if string is too long
        if (str.length() > sub_width)
            str = str.substr(str.length() - sub_width);

        // Right-align the string
        std::ostringstream ss;
        ss << std::setw(sub_width) << str;

        sub.render_widget(ss.str());
        end -= sub_width;
    }

    buf.get(0, 0).chr   = '[';
    buf.get(end, 0).chr = ']';

    size_t len = (end - 1);

    std::vector<size_t> w(this->_val.size());
    size_t last = 1;
    for (size_t j = 0; j < this->_val.size(); ++j) {
        size_t w = len * this->_val[j];
        for (size_t i = 0; i < w; ++i) {
            buf.get(i + last, 0).chr = '|';
            buf.get(i + last, 0).col = this->_cols[j];
        }
        last += w;
    }
}
