-- Vector prototype
vec = {
    x = 0,
    y = 0,
    z = 0
}

vec.__index = vec

-- Construct a new Vector
function vec.new(args)
    o = {}

    setmetatable(o, vec)

    if (args) then
        o.x = args.x or args[1] or 0
        o.y = args.y or args[2] or 0
        o.z = args.z or args[3] or 0
    end

    return o
end

-- Main Vector Constructor
vector = vec.new

-- Add two Vectors
function vec.__add(a, b)
    return {
        x = a.x + b.x,
        y = a.y + b.y,
        z = a.z + b.z
    }
end

-- Substract two Vectors
function vec.__sub(a, b)
    return {
        x = a.x - b.x,
        y = a.y - b.y,
        z = a.z - b.z
    }
end

-- Multiply two Vectors
function vec.__mul(a, b)
    return {
        x = a.x * b.x,
        y = a.y * b.y,
        z = a.z * b.z
    }
end

-- Divide two Vectors
function vec.__div(a, b)
    return {
        x = a.x / b.x,
        y = a.y / b.y,
        z = a.z / b.z
    }
end

-- Dotproduct of two Vectors
function vec.dot(a, b)
    return a.x * b.x + a.y * b.y + a.z * b.z
end

-- Crossproduct of two Vectors
function vec.cross(a, b)
    return vector {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}
end

-- The Length of a Vector
function vec.len(a)
    return math.sqrt(a:dot(a))
end

-- Normalize a Vector
function vec.normalize(a)
    l = a:len()

    return vector {a.x / l, a.y / l, a.z / l}
end
