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
local Bar = widget:new({
    percentaje = 0.,
    show_number = false,

    render = function(this, buffer)
        local x, _ = buffer:get_size()
        if this.show_number and x > 10 then
            buffer:get_sub(x - 5, 1, 5, 1):render(this.percentaje)
            x = x - 7
        end

        buffer:set(1, 1, '[');
        buffer:set(x, 1, ']');
        for i = 2, (x - 0) * this.percentaje, 1 do
            buffer:set(i, 1, '|', { type = "bit", r = 1, g = 0, b = 0 })
        end
    end
})

---@class Widget
return widget:new {
    bars = {
        hundrets = Bar:new(),
        thousands = Bar:new(),
    },
    render = function(this, buffer)
        buffer:get_sub(1, 1, 10, 1):render('fps : ' .. state.fps);
        buffer:get_sub(1, 2, 10, 1):render('tick: ' .. state.tick);
        this.bars.hundrets.show_number = true
        this.bars.thousands.show_number = true
        this.bars.hundrets:render(buffer:get_sub(1, 3, 50, 1))
        this.bars.thousands:render(buffer:get_sub(1, 4, 50, 1))
    end,

    update = function(this)
        local tick                     = state.tick

        this.bars.thousands.percentaje = 0.5;
    end
}
