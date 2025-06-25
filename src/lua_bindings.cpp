#include <cstdio>

#include <ly/exceptions.hpp>
#include <ly/render/buffer.hpp>
#include <ly/render/lua_bindings.hpp>
#include <ly/render/widgets.hpp>

#include <iostream>
#include <unordered_map>
#include <utility>

using namespace ly::render;

// ----------[value]----------
lua::Value::Value()
    : _type(Ty::None), _data(std::monostate{}) {}

lua::Value::Value(int64_t val)
    : _type(Ty::Integer), _data(val) {}

lua::Value::Value(bool val)
    : _type(Ty::Boolean), _data(val) {}

lua::Value::Value(double val)
    : _type(Ty::Float), _data(val) {}

lua::Value::Value(const std::string& val)
    : _type(Ty::String), _data(val) {}

lua::Value::Value(std::string&& val)
    : _type(Ty::String), _data(std::move(val)) {}

lua::Value::Value(const MapType& val)
    : _type(Ty::Map), _data(val) {}

lua::Value::Value(MapType&& val)
    : _type(Ty::Map), _data(std::move(val)) {}

lua::Value::Value(const ArrayType& val)
    : _type(Ty::Array), _data(val) {}

lua::Value::Value(ArrayType&& val)
    : _type(Ty::Array), _data(std::move(val)) {}

lua::Value lua::Value::none() {
    return lua::Value();
}

lua::Value lua::Value::integer(int64_t val) {
    return lua::Value(val);
}

lua::Value lua::Value::boolean(bool val) {
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
    return this->_type;
}

bool lua::Value::is_none() const {
    return this->_type == Ty::None;
}

bool lua::Value::is_integer() const {
    return this->_type == Ty::Integer;
}

bool lua::Value::is_boolean() const {
    return this->_type == Ty::Boolean;
}

bool lua::Value::is_float() const {
    return this->_type == Ty::Float;
}

bool lua::Value::is_string() const {
    return this->_type == Ty::String;
}

bool lua::Value::is_map() const {
    return this->_type == Ty::Map;
}

bool lua::Value::is_array() const {
    return this->_type == Ty::Array;
}

int64_t lua::Value::as_integer() const {
    if (this->_type != Ty::Integer) {
        LY_THROW("lua::Value is not an integer");
    }
    return std::get<int64_t>(_data);
}

bool lua::Value::as_boolean() const {
    if (this->_type != Ty::Boolean) {
        if (this->_type == Ty::None)
            return false;
        LY_THROW("lua::Value is not an boolean");
    }
    return std::get<bool>(_data);
}

double lua::Value::as_float() const {
    if (this->_type != Ty::Float) {
        LY_THROW("lua::Value is not a float");
    }
    return std::get<double>(_data);
}

const std::string& lua::Value::as_string() const {
    if (this->_type != Ty::String) {
        LY_THROW("lua::Value is not a string");
    }
    return std::get<std::string>(_data);
}

const lua::Value::MapType& lua::Value::as_map() const {
    if (this->_type != Ty::Map) {
        LY_THROW("lua::Value is not a map");
    }
    return std::get<MapType>(_data);
}

lua::Value::MapType& lua::Value::as_map() {
    if (this->_type != Ty::Map) {
        LY_THROW("lua::Value is not a map");
    }
    return std::get<MapType>(_data);
}

const lua::Value::ArrayType& lua::Value::as_array() const {
    if (this->_type != Ty::Array) {
        LY_THROW("lua::Value is not an array");
    }
    return std::get<ArrayType>(_data);
}

lua::Value::ArrayType& lua::Value::as_array() {
    if (this->_type != Ty::Array) {
        LY_THROW("lua::Value is not an array");
    }
    return std::get<ArrayType>(_data);
}

lua::Value& lua::Value::operator[](const std::string& key) {
    if (this->_type != Ty::Map) {
        LY_THROW("lua::Value is not a map");
    }

    auto& map = std::get<MapType>(_data);
    auto it   = map.find(key);
    if (it == map.end()) {
        auto [it, ok] =
            map.insert_or_assign(key, Value::none());
        if (!ok) {
            LY_THROW("failed to update state");
        }
        return it->second;
    }

    return it->second;
}

const lua::Value& lua::Value::operator[](
    const std::string& key) const {
    if (this->_type != Ty::Map) {
        LY_THROW("lua::Value is not a map");
    }

    auto& map = std::get<MapType>(this->_data);
    auto it   = map.find(key);
    if (it == map.end()) {
        const static Value _None = Value::none();
        return _None;
    }

    return it->second;
}

lua::Value& lua::Value::operator[](size_t index) {
    if (this->_type != Ty::Array) {
        LY_THROW("lua::Value is not an array");
    }
    return std::get<ArrayType>(this->_data)[index];
}

const lua::Value& lua::Value::operator[](
    size_t index) const {
    if (this->_type != Ty::Array) {
        LY_THROW("lua::Value is not an array");
    }
    return std::get<ArrayType>(this->_data)[index];
}

size_t lua::Value::size() const {
    switch (this->_type) {
    case Ty::Map:
        return std::get<MapType>(this->_data).size();
    case Ty::Array:
        return std::get<ArrayType>(this->_data).size();
    case Ty::String:
        return std::get<std::string>(this->_data).size();
    default:
        LY_THROW("lua::Value type does not support size()");
    }
}

bool lua::Value::empty() const {
    switch (_type) {
    case Ty::None:
        return true;
    case Ty::Map:
        return std::get<MapType>(this->_data).empty();
    case Ty::Array:
        return std::get<ArrayType>(this->_data).empty();
    case Ty::String:
        return std::get<std::string>(this->_data).empty();
    default:
        return false;
    }
}

bool lua::Value::operator==(const lua::Value& other) const {
    if (this->_type != other._type)
        return false;
    return this->_data == other._data;
}

bool lua::Value::operator!=(const lua::Value& other) const {
    return !(*this == other);
}

static void _push_value(
    lua_State* L, const lua::Value& val) {
    using namespace lua;
    using Ty = Value::Ty;

    switch (val.type()) {
    case Ty::None:
        lua_pushnil(L);
        break;

    case Ty::Integer:
        lua_pushinteger(L, val.as_integer());
        break;

    case Ty::Boolean:
        lua_pushboolean(L, val.as_boolean());
        break;

    case Ty::Float:
        lua_pushnumber(L, val.as_float());
        break;

    case Ty::String:
        lua_pushlstring(L, val.as_string().c_str(),
            val.as_string().size());
        break;

    case Ty::Map: {
        lua_newtable(L);
        for (const auto& [key, v] : val.as_map()) {
            _push_value(L, v);
            lua_setfield(L, -2, key.c_str());
        }
        break;
    }

    case Ty::Array: {
        lua_newtable(L);
        const auto& arr = val.as_array();
        for (size_t i = 0; i < arr.size(); ++i) {
            _push_value(L, arr[i]);
            lua_rawseti(L, -2,
                static_cast<int>(
                    i + 1)); // Lua arrays are 1-based
        }
        break;
    }
    }
}

static lua::Value to_value(lua_State* L, int index) {
    using namespace lua;
    switch (lua_type(L, index)) {
    case LUA_TNIL:
        return Value::none();

    case LUA_TNUMBER:
        if (lua_isinteger(L, index))
            return lua::Value::integer(
                (int64_t)lua_tointeger(L, index));
        else
            return lua::Value::float_val(
                lua_tonumber(L, index));

    case LUA_TBOOLEAN:
        return Value::boolean(lua_toboolean(L, index));

    case LUA_TSTRING:
        return Value::string(lua_tostring(L, index));

    case LUA_TTABLE: {
        Value::MapType map;
        Value::ArrayType array;

        bool is_array = true;
        size_t count  = 0;

        lua_pushnil(L);
        while (lua_next(L, index < 0 ? index - 1 : index) !=
               0) {
            ++count;

            if (lua_type(L, -2) == LUA_TSTRING) {
                is_array = false;
            }
            else if (lua_type(L, -2) == LUA_TNUMBER) {
                if (!lua_isinteger(L, -2))
                    is_array = false;
                else {
                    lua_Integer k = lua_tointeger(L, -2);
                    if (k !=
                        static_cast<lua_Integer>(count))
                        is_array = false;
                }
            }
            else {
                is_array = false;
            }

            if (is_array) {
                array.push_back(to_value(L, -1));
            }
            else {
                std::string key = lua_tostring(L, -2);
                map.insert_or_assign(key, to_value(L, -1));
            }

            lua_pop(L, 1); // pop value, keep key
        }

        return is_array ? Value::array(std::move(array))
                        : Value::map(std::move(map));
    }

    default:
        return Value::none(); // Unsupported types are
                              // treated as None
    }
}

std::ostream& lua::operator<<(
    std::ostream& os, const lua::Value& val) {
    using Ty = ly::render::lua::Value::Ty;

    // TODO: make this a flag
    const bool show_type = false;

    // TODO: finish this match
    switch (val._type) {
    case Ty::None:
        os << (show_type ? "None" : "");
        break;
    case Ty::Integer:
        if (show_type)
            os << "Integer: ";
        os << std::get<int64_t>(val._data);
        break;
    case Ty::Boolean:
        if (show_type)
            os << "Boolean: ";
        os << std::boolalpha << std::get<bool>(val._data);
        break;
    case Ty::Float:
        if (show_type)
            os << "Float: ";
        os << std::get<double>(val._data);
        break;
    case Ty::String:
        if (show_type)
            os << "String: ";
        os << std::get<std::string>(val._data);
        break;
    default:
        break;
    }

    return os;
}

// ----------[buffer]----------
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

static int _buffer_render(lua_State* L) {
    if (lua_isuserdata(L, 2)) {
        // TODO: handle userdata
        std::cerr << "userdata not handled yet";
        return 0;
    }
    if (lua_istable(L, 2)) {
        // TODO: handle tables in a semi well done way
        lua_getfield(L, 2, "render");
        lua_pushvalue(L, 2);
        lua_pushvalue(L, 1);
        lua_call(L, 2, 0);
        lua_pop(L, 1);
        return 0;
    }

    void* udata  = lua_touserdata(L, 1);
    Buffer* data = nullptr;

    if (luaL_testudata(L, 1, "BufferRef")) {
        data = static_cast<BufferRef*>(udata)->ptr;
    }
    else {
        data = static_cast<Buffer*>(udata);
    }
    lua::Value val = to_value(L, 2);
    data->render_widget(val);

    return 0;
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
    // this are indices
    size_t x = luaL_checkinteger(L, 2) - 1;
    size_t y = luaL_checkinteger(L, 3) - 1;
    // this are dimentions
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
        LY_THROW("Buffer is null");

    size_t x   = lua_tointeger(L, 2) - 1;
    size_t y   = lua_tointeger(L, 3) - 1;
    auto& unit = data->get(x, y);

    int top = lua_gettop(L);

    if (top >= 4 && lua_type(L, 4) == LUA_TSTRING) {
        unit.data = lua_tostring(L, 4);
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
            r       = (r & 1) << 0;
            g       = (g & 1) << 1;
            b       = (b & 1) << 2;
            unit.fc = ConsoleColor(r | g | b);
        }
        else if (ty == "8bit") {
            unit.fc = ConsoleColor(Color<ly::u8>(
                r & 0xff, g & 0xff, b & 0xff));
        }
    }

    return 0;
}

static void init_buffer_ref_metatable(lua_State* L) {
    luaL_newmetatable(L, "BufferRef");

    lua_pushcfunction(L, _buffer_set);
    lua_setfield(L, -2, "set");

    lua_pushcfunction(L, _buffer_get_size);
    lua_setfield(L, -2, "get_size");

    lua_pushcfunction(L, _buffer_get_sub);
    lua_setfield(L, -2, "get_sub");

    lua_pushcfunction(L, _buffer_render);
    lua_setfield(L, -2, "render");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1);
}

static void init_buffer_metatable(lua_State* L) {
    luaL_newmetatable(L, "Buffer");

    lua_pushcfunction(L, _buffer_set);
    lua_setfield(L, -2, "set");

    lua_pushcfunction(L, _buffer_get_size);
    lua_setfield(L, -2, "get_size");

    lua_pushcfunction(L, _buffer_get_sub);
    lua_setfield(L, -2, "get_sub");

    lua_pushcfunction(L, _buffer_render);
    lua_setfield(L, -2, "render");

    lua_pushcfunction(L, _buffer_gc);
    lua_setfield(L, -2, "__gc");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1);
}

// ----------[widget]----------
lua::LuaWidget::LuaWidget(std::weak_ptr<lua_State> L)
    : _L(L) {
    auto L_locked = this->_L.lock();
    auto Lg       = L_locked.get();

    int top = lua_gettop(Lg);
    if (top == 0)
        LY_THROW("no widget returned");

    if (!lua_istable(Lg, -1) && !lua_isuserdata(Lg, -1))
        LY_THROW("returned value is not table/userdata");

    if (!lua_getmetatable(Lg, -1))
        LY_THROW("widget has no metatable");

    lua_pop(Lg, 1);

    this->_ref = luaL_ref(Lg, LUA_REGISTRYINDEX);
}

lua::LuaWidget::LuaWidget(lua::LuaWidget&& W)
    : _L(W._L), _ref(W._ref) {}

lua::LuaWidget& lua::LuaWidget::operator=(
    lua::LuaWidget&& other) {
    this->_L   = other._L;
    this->_ref = other._ref;
    other._ref = LUA_NOREF;
    return *this;
}

lua::LuaWidget::~LuaWidget() {
    auto L_lock = this->_L.lock();
    if (_ref != LUA_NOREF) {
        luaL_unref(
            L_lock.get(), LUA_REGISTRYINDEX, this->_ref);
    }
}

void lua::LuaWidget::update() {
    auto L_lock = this->_L.lock();
    auto Lg     = L_lock.get();
    lua_rawgeti(Lg, LUA_REGISTRYINDEX, this->_ref);
    lua_getfield(Lg, -1, "update");
    lua_pushvalue(Lg, -2);
    lua_call(Lg, 1, 0);
    lua_pop(Lg, lua_gettop(Lg));
}

void lua::LuaWidget::render(Buffer& buf) const {
    auto L_lock = this->_L.lock();
    auto Lg     = L_lock.get();
    lua_rawgeti(Lg, LUA_REGISTRYINDEX, this->_ref);
    lua_getfield(Lg, -1, "render");
    lua_pushvalue(Lg, -2);

    BufferRef* ref = static_cast<BufferRef*>(
        lua_newuserdata(Lg, sizeof(BufferRef)));
    ref->ptr = &buf;
    luaL_getmetatable(Lg, "BufferRef");
    lua_setmetatable(Lg, -2);

    if (lua_pcall(Lg, 2, 0, 0) != LUA_OK) {
        const char* err = lua_tostring(Lg, -1);
        std::cerr << "Lua error: "
                  << (err ? err : "(unknown error)")
                  << std::endl;
        lua_pop(Lg, 1); // remove error message
    }

    lua_pop(Lg, 1);
}

// params self, table
static int _widget_extend(lua_State* L) {
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "super");

    lua_pushvalue(L, -2);
    lua_setmetatable(L, -2);

    return 1;
}

static int _widget_new(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    size_t top = lua_gettop(L);
    if (top < 2) {
        lua_createtable(L, 0, 0);
    }

    lua_pushvalue(L, 1);
    lua_setmetatable(L, -2);

    lua_pushstring(L, "Widget");
    lua_setfield(L, -2, "_type");

    lua_pushnumber(L, 1.0);
    lua_setfield(L, -2, "_version");

    return 1;
}

static int _widget_render(lua_State* L) {
    return 0;
}

static int _widget_update(lua_State* L) {
    return 0;
}

static void init_widget_metatable(lua_State* L) {
    static const luaL_Reg widget_methods[] = {
        {"extend", _widget_extend},
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

// ----------[events & state]----------
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
static int _state_index(lua_State* L) {
    auto* state = static_cast<lua::State*>(
        lua_touserdata(L, lua_upvalueindex(1)));
    if (!state)
        return luaL_error(
            L, "internal error: null state pointer");

    const char* key = lua_tostring(L, 2);
    if (!key)
        return luaL_error(
            L, "__index expects a string key");

    auto val = state->get_data(key);
    _push_value(L, val);
    return 1;
}

static int _state_newindex(lua_State* L) {
    auto* state = static_cast<lua::State*>(
        lua_touserdata(L, lua_upvalueindex(1)));
    if (!state)
        return luaL_error(
            L, "internal error: null state pointer");

    const char* key = lua_tostring(L, 2);
    if (!key)
        return luaL_error(
            L, "__newindex expects a string key");

    auto val = to_value(L, 3);
    state->set_data(key, val);

    return 0;
}

static void init_state_table(
    lua::State& cpp_state, lua_State* L) {
    using namespace lua;
    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, _state_on_event);
    lua_setfield(L, -2, "on_event");

    lua_createtable(L, 0, 2);

    lua_pushlightuserdata(L, &cpp_state);
    lua_pushcclosure(L, _state_index, 1);
    lua_setfield(L, -2, "__index");

    lua_pushlightuserdata(L, &cpp_state);
    lua_pushcclosure(L, _state_newindex, 1);
    lua_setfield(L, -2, "__newindex");

    lua_setmetatable(L, -2);

    lua_setglobal(L, "state");
}

bool lua::State::should_exit() {
    return this->get_data("exit").as_boolean();
}

void lua::State::press(char keycode) {
    auto it = _events.find("keypress");
    if (it == _events.end()) {
        LY_THROW("event is not defined");
        return;
    }

    int ref = it->second;

    lua_rawgeti(this->_L.get(), LUA_REGISTRYINDEX, ref);

    std::string k(1, keycode);
    lua_pushstring(this->_L.get(), k.c_str());

    if (lua_pcall(this->_L.get(), 1, 0, 0) != LUA_OK) {
        const char* err = lua_tostring(this->_L.get(), -1);
        fprintf(stderr, "Lua keypress handler error: %s\n",
            err);
        lua_pop(this->_L.get(), 1);
    }
}

void lua::State::set_data(std::string key, Value val) {
    this->_data[key] = val;
}

const lua::Value& lua::State::get_data(
    std::string key) const {
    auto& val = this->_data[key];
    return val;
}

lua::State::State() {
    using namespace lua;
    auto L_  = luaL_newstate();
    this->_L = std::shared_ptr<lua_State>(
        L_, State::LuaStateDeleter{});

    luaL_openlibs(this->_L.get());
    // luaL_requiref(L, "base", luaopen_base, true);
    // luaL_requiref(L, "math", luaopen_math, true);
    // luaL_requiref(L, "table", luaopen_table, true);
    // luaL_requiref(L, "package", luaopen_package, true);
    // luaL_requiref(L, "string", luaopen_string, true);

    init_buffer_metatable(this->_L.get());
    init_buffer_ref_metatable(this->_L.get());
    init_widget_metatable(this->_L.get());
    init_state_table(*this, this->_L.get());
};

lua::LuaWidget lua::State::from_file(std::string file) {
    lua_State* L = this->_L.get();

    if (luaL_dofile(L, file.c_str()) != LUA_OK) {
        const char* err =
            lua_tostring(L, -1); // get error message
        std::string message =
            "could not generate widget from script: ";
        message += (err ? err : "unknown error");
        lua_pop(
            L, 1); // remove error message from the stack
        LY_THROW(message);
    }

    LuaWidget _widget(this->_L);
    return _widget;
}

void lua::LuaWidget::debug_print() const {
    auto L_lock = this->_L.lock();
    auto Lg     = L_lock.get();

    lua_rawgeti(Lg, LUA_REGISTRYINDEX,
        this->_ref); // Push the widget table

    std::printf(
        "LuaWidget table at: %p\n", lua_topointer(Lg, -1));

    lua_pushnil(Lg); // Start table iteration

    while (lua_next(Lg, -2)) {
        // Key at -2, value at -1

        // Print key
        std::string keyStr;
        if (lua_type(Lg, -2) == LUA_TSTRING)
            keyStr = lua_tostring(Lg, -2);
        else
            keyStr = luaL_typename(Lg, -2);

        // Print value address
        const void* addr    = lua_topointer(Lg, -1);
        const char* valType = luaL_typename(Lg, -1);

        std::cout << "  [" << keyStr.c_str() << "] ("
                  << valType << ") => " << addr << '\n';

        lua_pop(Lg,
            1); // Remove value, keep key for next
                // iteration
    }

    lua_pop(Lg, 1); // Pop table
}
