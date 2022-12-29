require "res.scripts.libs.vector"

local function search(k, plist)
    for i = 1, #plist do
        local v = plist[i][k]
        -- try 'i'-th superclass
        if v then
            return v
        end
    end
end

-- Class prototype
clss = {
    lookDir = vector(),
    front = vector(),
    up = vector(),
    right = vector(),

    position = vector(),
    rotation = vector()
}

clss.__index = clss

-- Class Constructor
function clss.new(...)
    o = {}
    parents = {clss, ...}

    setmetatable(o, {
        __index = function(t, k)
            return search(k, parents)
        end
    })

    return o
end

-- Main Class Constructor
class = clss.new

-- Move Class
function clss.move(c, amount)
    c.position = c.position + c.front * vector(amount)
    c.position = c.position + c.up * vector(amount)
    c.position = c.position + c.right * vector(amount)
end

-- Rotate Class
function clss.rotate(c, amount)
    c.rotation = c.rotation + vector(amount)
end
