#ifndef __LY_STATE_HPP__
#define __LY_STATE_HPP__

#include <ostream>
#include <unordered_map>
#include <variant>
#include <vector>

#include "./ty.hpp"

namespace ly {

class Value;

class Value {
public:
    enum class Ty {
        None,
        Boolean,
        Integer,
        Float,
        String,
        Map,
        Array,
    };
    using MapType = std::unordered_map<std::string, Value>;
    using ArrayType = std::vector<Value>;

private:
    Ty _type;
    std::variant<std::monostate, // None
        int64_t,                 // Integer
        bool,                    // bool
        double,                  // Float
        std::string,             // String
        MapType,                 // Map
        ArrayType                // Array
        >
        _data;

    // Private constructors
    Value();
    Value(bool val);
    Value(int64_t val);
    Value(double val);
    Value(const std::string& val);
    Value(std::string&& val);
    Value(const MapType& val);
    Value(MapType&& val);
    Value(const ArrayType& val);
    Value(ArrayType&& val);

public:
    // Copy and move constructors
    Value(const Value& other) = default;
    Value(Value&& other)      = default;

    Value& operator=(const Value& other) = default;
    Value& operator=(Value&& other)      = default;

    // factory functions
    static Value none();
    static Value boolean(bool val);
    static Value integer(int64_t val);
    static Value integer(int val);
    static Value float_val(double val);
    static Value float_val(float val);
    static Value string(const std::string& val);
    static Value string(std::string&& val);
    static Value string(const char* val);
    static Value map();
    static Value map(const MapType& val);
    static Value map(MapType&& val);
    static Value array();
    static Value array(const ArrayType& val);
    static Value array(ArrayType&& val);

    template <typename... Args>
    static Value array(Args&&... args);

    Ty type() const;
    bool is_none() const;
    bool is_integer() const;
    bool is_boolean() const;
    bool is_float() const;
    bool is_string() const;
    bool is_map() const;
    bool is_array() const;

    int64_t as_integer() const;
    bool as_boolean() const;
    double as_float() const;
    const std::string& as_string() const;
    const MapType& as_map() const;
    MapType& as_map();
    const ArrayType& as_array() const;
    ArrayType& as_array();
    Value& operator[](const std::string& key);
    const Value& operator[](const std::string& key) const;

    Value& operator[](size_t index);
    const Value& operator[](size_t index) const;

    size_t size() const;
    bool empty() const;

    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;
    friend std::ostream& operator<<(
        std::ostream& os, const Value& val);
};

template <typename... Args>
Value Value::array(Args&&... args) {
    return Value(ArrayType{std::forward<Args>(args)...});
}

class IntoValue {
public:
    virtual ~IntoValue() {}
    virtual Value into() = 0;
};

class State {
private:
public:
};

} // namespace ly

std::ostream& operator<<(
    std::ostream& os, const ly::Value& val);

#endif
