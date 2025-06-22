---@class ColorAnsi
---@field type "bit"
---@field R 0 | 1
---@field G 0 | 1
---@field B 0 | 1

---@class Color8bit
---@field type "8bit"
---@field R integer @[0,255]
---@field G integer @[0,255]
---@field B integer @[0,255]

---@alias Color ColorAnsi | Color8bit

---@class Buffer
---@field set function

---@class Widget
---@field new function

local letters = ''

state.on_event('keypress', function(key)
    if key == 'q' then
        state.exit = true;
        return
    end
    letters = letters .. key
end)


---@class Widget
return widget:new {
    render = function(_, buffer)
        buffer:get_sub(10, 10, 10, 10):render(letters);
    end
}
