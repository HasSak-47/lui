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
        local x, _ = buffer:get_size();
        for ij = 0, string.len(letters) - 1, 1 do
            local i = 1 + (ij % x);
            local j = 1 + (ij // x);
            buffer:set(i, j, letters:sub(ij, ij))
        end
    end
}
