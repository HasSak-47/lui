# Footguns
* Using printf to enter and exit alternate screen and moving the cursor
* The interaction between the Value class and lua

# features
* improve error handling so it just doesn't just crash
    * add a warning and error widget
* utf-8 characters
* improve lua lsp support
* widgets
    * add back some widgets to c++ and expose them to lua
    * a general interface to expose c++ widgets to lua
* handle resizing of screen

# chore 
* finish function _buffer_render
    * src/lua_bindings.cpp:483 handle user data
    * src/lua_bindings.cpp:488 handle tables
* finish printing of Value 
    * src/lua_bindings.cpp:418 handle tables

