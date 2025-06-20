widget = {
    new = function(...) end,
    render = function(...) end,
    update = function(...) end,
}

state = {
    __index = function()
        return 1
    end
}
