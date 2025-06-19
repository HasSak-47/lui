#include <cstdio>
#include <cstdlib>
#include <ly/render/buffer.hpp>
#include <ly/render/lua_bindings.hpp>
#include <ly/render/widgets.hpp>
#include <stdexcept>
#include <unordered_map>

lua_State* ly::render::lua::L = nullptr;

using namespace ly::render;

// ----[value]----
lua::Value::Value()
    : type_(Ty::None), data_(std::monostate{}) {}

lua::Value::Value(int64_t val)
    : type_(Ty::Integer), data_(val) {}

lua::Value::Value(double val)
    : type_(Ty::Float), data_(val) {}

lua::Value::Value(const std::string& val)
    : type_(Ty::String), data_(val) {}

lua::Value::Value(std::string&& val)
    : type_(Ty::String), data_(std::move(val)) {}

lua::Value::Value(const MapType& val)
    : type_(Ty::Map), data_(val) {}

lua::Value::Value(MapType&& val)
    : type_(Ty::Map), data_(std::move(val)) {}

lua::Value::Value(const ArrayType& val)
    : type_(Ty::Array), data_(val) {}

lua::Value::Value(ArrayType&& val)
    : type_(Ty::Array), data_(std::move(val)) {}

lua::Value lua::Value::none() {
    return lua::Value();
}

lua::Value lua::Value::integer(int64_t val) {
    return lua::Value(val);
}

lua::Value lua::Value::integer(int val) {
    return Value(static_cast<int64_t>(val));
}

lua::Value lua::Value::float_val(double val) {
    return Value(val);
}

lua::Value lua::Value::float_val(float val) {
    return Value(static_cast<double>(val));
}

lua::Value lua::Value::string(const std::string& val) {
    return Value(val);
}

lua::Value lua::Value::string(std::string&& val) {
    return Value(std::move(val));
}

lua::Value lua::Value::string(const char* val) {
    return Value(std::string(val));
}

lua::Value lua::Value::map() {
    return Value(MapType{});
}

lua::Value lua::Value::map(const MapType& val) {
    return Value(val);
}

lua::Value lua::Value::map(MapType&& val) {
    return Value(std::move(val));
}

lua::Value lua::Value::array() {
    return Value(ArrayType{});
}

lua::Value lua::Value::array(const ArrayType& val) {
    return Value(val);
}

lua::Value lua::Value::array(ArrayType&& val) {
    return Value(std::move(val));
}

lua::Value::Ty lua::Value::type() const {
    return type_;
}

bool lua::Value::is_none() const {
    return type_ == Ty::None;
}

bool lua::Value::is_integer() const {
    return type_ == Ty::Integer;
}

bool lua::Value::is_float() const {
    return type_ == Ty::Float;
}

bool lua::Value::is_string() const {
    return type_ == Ty::String;
}

bool lua::Value::is_map() const {
    return type_ == Ty::Map;
}

bool lua::Value::is_array() const {
    return type_ == Ty::Array;
}

int64_t lua::Value::as_integer() const {
    if (type_ != Ty::Integer) {
        throw std::runtime_error(
            "lua::Value is not an integer");
    }
    return std::get<int64_t>(data_);
}

double lua::Value::as_float() const {
    if (type_ != Ty::Float) {
        throw std::runtime_error(
            "lua::Value is not a float");
    }
    return std::get<double>(data_);
}

const std::string& lua::Value::as_string() const {
    if (type_ != Ty::String) {
        throw std::runtime_error(
            "lua::Value is not a string");
    }
    return std::get<std::string>(data_);
}

const lua::Value::MapType& lua::Value::as_map() const {
    if (type_ != Ty::Map) {
        throw std::runtime_error("lua::Value is not a map");
    }
    return std::get<MapType>(data_);
}

lua::Value::MapType& lua::Value::as_map() {
    if (type_ != Ty::Map) {
        throw std::runtime_error("lua::Value is not a map");
    }
    return std::get<MapType>(data_);
}

const lua::Value::ArrayType& lua::Value::as_array() const {
    if (type_ != Ty::Array) {
        throw std::runtime_error(
            "lua::Value is not an array");
    }
    return std::get<ArrayType>(data_);
}

lua::Value::ArrayType& lua::Value::as_array() {
    if (type_ != Ty::Array) {
        throw std::runtime_error(
            "lua::Value is not an array");
    }
    return std::get<ArrayType>(data_);
}

lua::Value& lua::Value::operator[](const std::string& key) {
    if (type_ != Ty::Map) {
        throw std::runtime_error("lua::Value is not a map");
    }
    return std::get<MapType>(data_)[key];
}

const lua::Value& lua::Value::operator[](
    const std::string& key) const {
    if (type_ != Ty::Map) {
        throw std::runtime_error("lua::Value is not a map");
    }
    return std::get<MapType>(data_).at(key);
}

lua::Value& lua::Value::operator[](size_t index) {
    if (type_ != Ty::Array) {
        throw std::runtime_error(
            "lua::Value is not an array");
    }
    return std::get<ArrayType>(data_)[index];
}

const lua::Value& lua::Value::operator[](
    size_t index) const {
    if (type_ != Ty::Array) {
        throw std::runtime_error(
            "lua::Value is not an array");
    }
    return std::get<ArrayType>(data_)[index];
}

size_t lua::Value::size() const {
    switch (type_) {
    case Ty::Map:
        return std::get<MapType>(data_).size();
    case Ty::Array:
        return std::get<ArrayType>(data_).size();
    case Ty::String:
        return std::get<std::string>(data_).size();
    default:
        throw std::runtime_error(
            "lua::Value type does not support size()");
    }
}

bool lua::Value::empty() const {
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

bool lua::Value::operator==(const lua::Value& other) const {
    if (type_ != other.type_)
        return false;
    return data_ == other.data_;
}

bool lua::Value::operator!=(const lua::Value& other) const {
    return !(*this == other);
}

// ----[buffer]----
struct BufferRef {
    Buffer* ptr;
};

static int _buffer_get_size(lua_State* L) {
    void* udata  = lua_touserdata(L, 1);
    Buffer* data = nullptr;

    if (luaL_testudata(L, 1, "BufferRef")) {
        data = static_cast<BufferRef*>(udata)->ptr;
    }
    else {
        data = static_cast<Buffer*>(udata);
    }

    lua_pushinteger(L, data->width());
    lua_pushinteger(L, data->height());
    return 2;
}

static int _buffer_get_sub(lua_State* L) {
    void* udata    = lua_touserdata(L, 1);
    Buffer* parent = nullptr;

    if (luaL_testudata(L, 1, "BufferRef")) {
        parent = static_cast<BufferRef*>(udata)->ptr;
    }
    else {
        parent = static_cast<Buffer*>(udata);
    }

    size_t x = luaL_checkinteger(L, 2);
    size_t y = luaL_checkinteger(L, 3);
    size_t w = luaL_checkinteger(L, 4);
    size_t h = luaL_checkinteger(L, 5);

    Buffer sub = parent->get_sub_buffer(x, y, w, h);

    void* mem = lua_newuserdata(L, sizeof(Buffer));
    new (mem) Buffer(std::move(sub));

    luaL_getmetatable(L, "Buffer");
    lua_setmetatable(L, -2);
    return 1;
}

static int _buffer_gc(lua_State* L) {
    Buffer* buf = static_cast<Buffer*>(
        luaL_checkudata(L, 1, "Buffer"));
    buf->~Buffer();
    return 0;
}

static int _buffer_set(lua_State* L) {
    void* udata  = lua_touserdata(L, 1);
    Buffer* data = nullptr;

    if (luaL_testudata(L, 1, "BufferRef")) {
        data = static_cast<BufferRef*>(udata)->ptr;
    }
    else {
        data = static_cast<Buffer*>(udata);
    }

    if (!data)
        throw std::runtime_error("Buffer is null");

    size_t x   = lua_tointeger(L, 2) - 1;
    size_t y   = lua_tointeger(L, 3) - 1;
    auto& unit = data->get(x, y);

    int top = lua_gettop(L);

    if (top >= 4 && lua_type(L, 4) == LUA_TSTRING) {
        const char* str = lua_tostring(L, 4);
        if (str && str[0])
            unit.chr = str[0];
    }

    int color_idx =
        (top >= 5)
            ? 5
            : ((top == 4 && lua_istable(L, 4)) ? 4 : 0);

    if (color_idx) {
        lua_getfield(L, color_idx, "type");
        lua_getfield(L, color_idx, "r");
        lua_getfield(L, color_idx, "g");
        lua_getfield(L, color_idx, "b");

        std::string ty = lua_tostring(L, -4);
        int r          = lua_tointeger(L, -3);
        int g          = lua_tointeger(L, -2);
        int b          = lua_tointeger(L, -1);
        lua_pop(L, 4);

        if (ty == "bit") {
            r        = (r & 1) << 0;
            g        = (g & 1) << 1;
            b        = (b & 1) << 2;
            unit.col = ConsoleColor(r | g | b);
        }
        else if (ty == "8bit") {
            unit.col = ConsoleColor(Color<ly::u8>(
                r & 0xff, g & 0xff, b & 0xff));
        }
    }

    return 0;
}

static void init_buffer_ref_metatable() {
    luaL_newmetatable(lua::L, "BufferRef");

    lua_pushcfunction(lua::L, _buffer_set);
    lua_setfield(lua::L, -2, "set");

    lua_pushcfunction(lua::L, _buffer_get_size);
    lua_setfield(lua::L, -2, "get_size");

    lua_pushcfunction(lua::L, _buffer_get_sub);
    lua_setfield(lua::L, -2, "get_sub");

    lua_pushvalue(lua::L, -1);
    lua_setfield(lua::L, -2, "__index");

    lua_pop(lua::L, 1);
}

static void init_buffer_metatable() {
    luaL_newmetatable(lua::L, "Buffer");

    lua_pushcfunction(lua::L, _buffer_set);
    lua_setfield(lua::L, -2, "set");

    lua_pushcfunction(lua::L, _buffer_get_size);
    lua_setfield(lua::L, -2, "get_size");

    lua_pushcfunction(lua::L, _buffer_get_sub);
    lua_setfield(lua::L, -2, "get_sub");

    lua_pushcfunction(lua::L, _buffer_gc);
    lua_setfield(lua::L, -2, "__gc");

    lua_pushvalue(lua::L, -1);
    lua_setfield(lua::L, -2, "__index");

    lua_pop(lua::L, 1);
}

// ----[widget]----
lua::LuaWidget::LuaWidget(std::string path) {
    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        std::string err = lua_tostring(L, -1);
        lua_pop(L, 1);
        throw std::runtime_error("Lua error: " + err);
    }

    int top = lua_gettop(L);
    if (top == 0)
        throw std::runtime_error("no widget returned");

    if (!lua_istable(L, -1) && !lua_isuserdata(L, -1))
        throw std::runtime_error(
            "returned value is not table/userdata");

    if (!lua_getmetatable(L, -1))
        throw std::runtime_error("widget has no metatable");

    luaL_getmetatable(L, "Widget");

    if (!lua_rawequal(L, -1, -2)) {
        lua_pop(L, 3);
        throw std::runtime_error(
            "widget metatable mismatch");
    }

    lua_pop(L, 2);

    this->_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

lua::LuaWidget::~LuaWidget() {
    if (_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, this->_ref);
    }
}

void lua::LuaWidget::update() {
    lua_rawgeti(lua::L, LUA_REGISTRYINDEX, this->_ref);
    lua_getfield(L, -1, "update");
    lua_call(L, 1, 0);
    lua_pop(L, lua_gettop(L));
}

void lua::LuaWidget::render(
    Buffer& buf, size_t tick) const {
    lua_rawgeti(lua::L, LUA_REGISTRYINDEX, this->_ref);
    lua_getfield(lua::L, -1, "render");
    lua_pushvalue(lua::L, -2);

    BufferRef* ref = static_cast<BufferRef*>(
        lua_newuserdata(lua::L, sizeof(BufferRef)));
    ref->ptr = &buf;
    luaL_getmetatable(lua::L, "BufferRef");
    lua_setmetatable(lua::L, -2);

    if (lua_pcall(lua::L, 2, 0, 0) != LUA_OK) {
        const char* err = lua_tostring(lua::L, -1);
        fprintf(stderr, "Lua render() error: %s\n", err);
        lua_pop(lua::L, 1);
    }

    lua_pop(lua::L, 1);
}

static int _widget_new(lua_State* L) {
    size_t size = lua_gettop(L);
    switch (size) {
    case 0:
        lua_createtable(L, 0, 0);
        break;
    case 1:
        if (!lua_istable(L, 1)) {
            luaL_error(L,
                "widget.new expects a table or no arguments");
        }
        break;
    }

    luaL_getmetatable(L, "Widget");
    lua_setmetatable(L, -2);

    return 1;
}

static int _widget_render(lua_State* L) {
    // empty by default
    return 0;
}

static int _widget_update(lua_State* L) {
    // empty by default
    return 0;
}

static void init_widget_metatable() {
    static const luaL_Reg widget_methods[] = {
        {   "new",    _widget_new},
        {"render", _widget_render},
        {"update", _widget_update},
        {    NULL,           NULL}
    };

    using namespace lua;
    luaL_newmetatable(L, "Widget");

    luaL_setfuncs(L, widget_methods, 0);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_newtable(L);
    luaL_setfuncs(L, widget_methods, 0);
    lua_setglobal(L, "widget");
}

// ----[Events]----
std::unordered_map<std::string, int> _events = {};

static int _state_on_event(lua_State* L) {
    const char* event = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    lua_pushvalue(L, 2);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);

    auto it = _events.find(event);
    if (it != _events.end()) {
        luaL_unref(L, LUA_REGISTRYINDEX, it->second);
    }

    _events[event] = ref;

    return 0;
}

void lua::handle_keypress(char keycode) {
    auto it = _events.find("keypress");
    if (it == _events.end()) {
        exit(1);
        return;
    }

    int ref = it->second;

    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

    // Push the argument (keycode as string)
    std::string k(
        1, keycode); // safer way to make 1-char string
    lua_pushstring(L, k.c_str());

    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        fprintf(stderr, "Lua keypress handler error: %s\n",
            err);
        lua_pop(L, 1); // Pop error
    }
}

bool lua::should_exit() {
    lua_getglobal(L, "state");
    lua_getfield(L, -1, "exit");
    bool k = lua_toboolean(L, -1);

    lua_pop(L, 2);

    return k;
}

static void init_state_table() {
    using namespace lua;
    lua_createtable(L, 1, 1);
    lua_pushcfunction(L, _state_on_event);
    lua_setfield(L, -2, "on_event");
    lua_setglobal(L, "state");
}

void lua::lua_init() {
    using namespace lua;
    L = luaL_newstate();
    luaL_openlibs(L);
    // luaL_requiref(L, "base", luaopen_base, true);
    // luaL_requiref(L, "math", luaopen_math, true);
    // luaL_requiref(L, "table", luaopen_table, true);
    // luaL_requiref(L, "package", luaopen_package, true);
    // luaL_requiref(L, "string", luaopen_string, true);

    init_buffer_metatable();
    init_buffer_ref_metatable();
    init_widget_metatable();
    init_state_table();
};
