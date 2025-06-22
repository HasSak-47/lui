# LUI 
A Terminal UI Rendering System. the engine is implemented in C++ with Lua scripting support. It is centered around a `Buffer` and `Widget` abstraction for colored character grids, C++ and Lua-defined widgets.

## Features
- **True color and 3-bit color**
- **Widget system** for modular, scriptable UI components written in Lua and C++.
- **Modular architecture** split into `buffer`, `lua_bindings`, `widgets`, and `window`.

* check `TODO.md` to see planed features or bugs!

## Build

### Requirements

* C++20-compatible compiler (GCC, Clang)
* Lua 5.4
* POSIX-compatible terminal (Linux/macOS)


### Process

- getting the stable code `git clone -b stable https://github.com/HasSak-47/lui.git`
- building
    - to build `make build`
    - run demo `make run`

## Lua Scripting

Lua scripts define UI widgets and handle events.

### Example `init.lua` (simplified)

```lua
local w = widget.new {
  render = function(self, buf)
    buf:set(1, 1, 'H')
    buf:set(2, 1, 'e')
    buf:set(3, 1, 'l')
    buf:set(4, 1, 'l')
    buf:set(5, 1, 'o')
    local sub = buf:get_sub(3, 3)
    sub:render('world!')
  end,

  update = function(self)
    -- Optional: logic per frame
  end
}

state.on_event("keypress", function(k)
  if k == "q" then
    state.exit = true
  end
end)

return w
```
## Runtime Behavior

* Enters **alternate screen buffer**
* Runs at \~60Hz unless blocked
* Accepts **keypresses** via raw stdin (non-blocking)
* Calls `render` and `update` on the Lua widget each frame

## Concepts

* `Buffer` represents a 2D grid of `Unit` (character + color).
* `Widget` is an C++ class with `void render(Buffer& buf) const` and `void update()` methods.
* `Renderable` represents a object that can be drawn to the screen either because it is a widget has overloaded the function `render(Buffer&, T val)` or can be streamed using `std::ostream& operator<< (...)`
* `LuaWidget` is a child class of widget that interfaces with lua tables that contain the functions `render(this, buf)` and `update(this, buf)` 
