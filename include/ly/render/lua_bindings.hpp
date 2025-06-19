#ifndef __RENDER_LUA_BINDINGS_HPP__
#define __RENDER_LUA_BINDINGS_HPP__

#include <lua.hpp>
#include <ly/render/widgets.hpp>
#include <unordered_map>

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

namespace ly::render::lua {

extern lua_State* L;

class Value {
public:
    enum class Ty {
        None,
        Integer,
        Float,
        String,
        Map,
        Array,
    };

private:
    using MapType = std::unordered_map<std::string, Value>;
    using ArrayType = std::vector<Value>;

    Ty type_;
    std::variant<std::monostate, // None
        int64_t,                 // Integer
        double,                  // Float
        std::string,             // String
        MapType,                 // Map
        ArrayType                // Array
        >
        data_;

    // Private constructors
    Value();
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

    // Assignment operators
    Value& operator=(const Value& other) = default;
    Value& operator=(Value&& other)      = default;

    // Static factory functions
    static Value none();
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

    // Type checking
    Ty type() const;
    bool is_none() const;
    bool is_integer() const;
    bool is_float() const;
    bool is_string() const;
    bool is_map() const;
    bool is_array() const;

    // Value accessors with type checking
    int64_t as_integer() const;
    double as_float() const;
    const std::string& as_string() const;
    const MapType& as_map() const;
    MapType& as_map();
    const ArrayType& as_array() const;
    ArrayType& as_array();

    // Convenience operators for map access
    Value& operator[](const std::string& key);
    const Value& operator[](const std::string& key) const;

    // Convenience operators for array access
    Value& operator[](size_t index);
    const Value& operator[](size_t index) const;

    // Utility methods
    size_t size() const;
    bool empty() const;

    // Equality operators
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;
};

template <typename... Args>
Value Value::array(Args&&... args) {
    return Value(ArrayType{std::forward<Args>(args)...});
}

void lua_init();

void handle_keypress(char keycode);
bool should_exit();

class State {
private:
    std::unordered_map<std::string, Value> _data = {};

public:
    State();
};

class LuaWidget : public widgets::Widget {
private:
    int _ref;

protected:
    LuaWidget(int ref);

    LuaWidget();

public:
    LuaWidget(std::string path);
    ~LuaWidget() override;

    void update() override;
    void render(
        Buffer& buf, size_t tick = 0) const override;

    friend LuaWidget from_table();
};

LuaWidget from_table();

} // namespace ly::render::lua

#endif
