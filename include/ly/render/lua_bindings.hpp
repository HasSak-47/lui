/*
#ifndef __RENDER_LUA_BINDINGS_HPP__
#define __RENDER_LUA_BINDINGS_HPP__

#include <functional>
#include <ly/render/widgets.hpp>

#include <lua.hpp>

#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>

namespace ly::render::lua {

class State;
class LuaWidget;

class State {
public:
    using Fn = std::function<int(lua_State*)>;

private:
    struct LuaStateDeleter {
        void operator()(lua_State* L) const {
            if (L)
                lua_close(L);
        }
    };

    Value _data = Value::map();
    std::shared_ptr<lua_State> _L;
    std::unordered_map<std::string, Fn> _funcs;

public:
    State();

    void press(char key);
    bool should_exit();

    void set_data(std::string key, Value val);
    void set_function(std::string key, Fn fn);
    const Value& get_data(std::string key) const;
    bool func_exitst(std::string key);
    Fn& get_func(std::string key);
    void debug_print() const;

    // LuaWidget
    LuaWidget from_file(std::string file);

    friend class LuaWidget;
};

class LuaWidget : public widgets::Widget {
private:
    std::weak_ptr<lua_State> _L;
    std::unordered_map<std::string, int> _events = {};
    int _ref;

protected:
    // takes the table in the stack
    LuaWidget(std::weak_ptr<lua_State> L);

public:
    LuaWidget(LuaWidget&& other);
    LuaWidget& operator=(LuaWidget&& other);
    ~LuaWidget() override;

    void update() override;
    void render(Buffer& buf) const override;
    void debug_print() const;

    friend class State;
};
} // namespace ly::render::lua
#endif
*/
