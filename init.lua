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
---@field render function
---@field update function

local letters = ''
state.on_event('keypress', function(key)
    if key == 'q' then
        state.exit = true;
        return
    end
    letters = letters .. key
end)

---@class Widget
local Bar = widget:extend({
    new = function(self)
        local t = self.super.new(self)
        setmetatable(t, self)

        t._type = "Bar"

        t.percentage = 0.
        t.show_number = false

        t.__index = self
        return t
    end,

    render = function(self, buffer)
        local x, _ = buffer:get_size()

        if self.show_number and x > 10 then
            local s = string.format("%.0f%%", self.percentage * 100)
            buffer:get_sub(x - 7, 1, 7, 1):render(s)
            x = x - 8
        end

        buffer:set(1, 1, '[')
        buffer:set(x, 1, ']')

        local bar_width = x - 2
        for i = 1, math.floor(bar_width * self.percentage) do
            buffer:set(i + 1, 1, '|', { type = "bit", r = 1, g = 0, b = 0 })
        end
    end
})

---@class Widget
local M_type = widget:extend {

    new = function(self)
        local t = self.super.new(self)
        setmetatable(t, self)
        t._type = "Bar"
        t.bars = {
            hundrets = Bar:new(),
            thousands = Bar:new(),
        }
        t.__index = self
        return t
    end,

    render = function(self, buffer)
        local x, y = buffer:get_size()
        buffer:get_sub(1, 1, x, 1):render('fps : ' .. state.fps);
        buffer:get_sub(1, 2, x, 1):render('tick: ' .. state.tick);
        buffer:get_sub(1, 3, x, 1):render(self.bars.thousands)
        buffer:get_sub(1, 4, x, 1):render(self.bars.hundrets)
        buffer:get_sub(1, 5, x, 1):render('utf8 string: ññññ');
    end,

    update = function(self)
        local tick = state.tick or 0
        self.bars.thousands.percentage = ((tick + self.bars.thousands.percentage) % 1000) / 1000
        self.bars.hundrets.percentage = ((tick + self.bars.hundrets.percentage) % 100) / 100
    end
}

M_type.__index = M_type

state.set_color("bg", {
    type = "8bit",
    r = 0,
    g = 0,
    b = 0,
});

return M_type:new();
