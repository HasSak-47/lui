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

---@class Unit
---@field chr string
---@field color Color

---@class Buffer
---@field cell Unit[][]

---@class Widget
---@field new function

---@class Widget
local Bar = widget:new {
    render = function(self, buffer)
        local x, _ = buffer:get_size();
        buffer:set(1, 1, '[', { type = "bit", r = 1, g = 1, b = 1 })
        buffer:set(x, 1, ']', { type = "bit", r = 1, g = 1, b = 1 })
        for i = 2, x - 1, 1 do
            buffer:set(x, 1, '|', { type = "bit", r = 1, g = 1, b = 1 })
        end
    end
};

local keys = ''

state.on_event('keypress', function(key)
    if key == 'q' then
        state.exit = true;
        return
    end
    keys = keys + key
end)


return widget:new {
    render = function(self, buffer)
        local x, _ = buffer:get_size();
        Bar:new(buffer:get_sub(0, 0, x, 1));
        Bar:new(buffer:get_sub(0, 1, x, 1));
    end
}
