local module = {}
module.vector2MetaTable = {}
module.vector3MetaTable = {}
module.vector4MetaTable = {}

---- Vector 2 ----
function module.vector2MetaTable.__add(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x + rhs, y = self.y + rhs }
    else
        r = { x = self.x + rhs.x, y = self.y + rhs.y }
    end
    setmetatable(r, module.vector2MetaTable)
    return r
end

function module.vector2MetaTable.__sub(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x - rhs, y = self.y - rhs }
    else
        r = { x = self.x - rhs.x, y = self.y - rhs.y }
    end
    setmetatable(r, module.vector2MetaTable)
    return r
end

function module.vector2MetaTable.__mul(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x * rhs, y = self.y * rhs }
    else
        r = { x = self.x * rhs.x, y = self.y * rhs.y }
    end
    setmetatable(r, module.vector2MetaTable)
    return r
end

function module.vector2MetaTable.__div(self, rhs)
    local r
    if type(b) == "number" then
        rhs = 1 / rhs
        r = { x = self.x * rhs, y = self.y * rhs }
    else
        r = { x = self.x / rhs.x, y = self.y / rhs.y }
    end
    setmetatable(r, module.vector2MetaTable)
    return r
end

function module.vector2MetaTable.__mod(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x % rhs, y = self.y % rhs }
    else
        r = { x = self.x % rhs.x, y = self.y % rhs.y }
    end
    setmetatable(r, module.vector2MetaTable)
    return r
end

function module.vector2MetaTable.__pow(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x ^ rhs, y = self.y ^ rhs }
    else
        r = { x = self.x ^ rhs.x, y = self.y ^ rhs.y }
    end
    setmetatable(r, module.vector2MetaTable)
    return r
end

function module.vector2MetaTable.__unm(a)
    local r
    r.x = -self.x
    r.y = -self.y
    setmetatable(r, module.vector2MetaTable)
    return r
end

function module.vector2MetaTable.SquaredLength(self, rhs)
    if rhs == nil then
        return self.x*self.x + self.y*self.y
    else
        local dx = self.x - rhs.x
        local dy = self.y - rhs.y
        return dx*dx + dy*dy
    end
end

function module.vector2MetaTable.Length(self, rhs)
    return math.sqrt(self:SquaredLength(rhs))
end

function module.vector2MetaTable.Normalize(self)
    return self / self:Length()
end

function module.vector2MetaTable.Dot(self, rhs)
    return self.x * rhs.x + self.y * rhs.y
end

---- Vector 3 ----
function module.vector3MetaTable.__add(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x + rhs, y = self.y + rhs, z = self.z + rhs }
    else
        r = { x = self.x + rhs.x, y = self.y + rhs.y, z = self.z + rhs.z }
    end
    setmetatable(r, module.vector3MetaTable)
    return r
end

function module.vector3MetaTable.__sub(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x - rhs, y = self.y - rhs, z = self.z - rhs }
    else
        r = { x = self.x - rhs.x, y = self.y - rhs.y, z = self.z - rhs.z }
    end
    setmetatable(r, module.vector3MetaTable)
    return r
end

function module.vector3MetaTable.__mul(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x * rhs, y = self.y * rhs, z = self.z * rhs }
    else
        r = { x = self.x * rhs.x, y = self.y * rhs.y, z = self.z * rhs.z }
    end
    setmetatable(r, module.vector3MetaTable)
    return r
end

function module.vector3MetaTable.__div(self, rhs)
    local r
    if type(b) == "number" then
        rhs = 1 / rhs
        r = { x = self.x * rhs, y = self.y * rhs, z = self.z * rhs }
    else
        r = { x = self.x / rhs.x, y = self.y / rhs.y, z = self.z / rhs.z }
    end
    setmetatable(r, module.vector3MetaTable)
    return r
end

function module.vector3MetaTable.__mod(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x % rhs, y = self.y % rhs, z = self.z % rhs }
    else
        r = { x = self.x % rhs.x, y = self.y % rhs.y, z = self.z % rhs.z }
    end
    setmetatable(r, module.vector3MetaTable)
    return r
end

function module.vector3MetaTable.__pow(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x ^ rhs, y = self.y ^ rhs, z = self.z ^ rhs }
    else
        r = { x = self.x ^ rhs.x, y = self.y ^ rhs.y, z = self.z ^ rhs.z }
    end
    setmetatable(r, module.vector3MetaTable)
    return r
end

function module.vector3MetaTable.__unm(a)
    local r
    r.x = -self.x
    r.y = -self.y
    r.z = -self.z
    setmetatable(r, module.vector3MetaTable)
    return r
end

function module.vector3MetaTable.SquaredLength(self, rhs)
    if rhs == nil then
        return self.x*self.x + self.y*self.y + self.z*self.z
    else
        local dx = self.x - rhs.x
        local dy = self.y - rhs.y
        local dz = self.z - rhs.z
        return dx*dx + dy*dy + dz*dz
    end
end

function module.vector3MetaTable.Length(self, rhs)
    return math.sqrt(self:SquaredLength(rhs))
end

function module.vector3MetaTable.Normalize(self)
    return self / self:Length()
end

function module.vector3MetaTable.Dot(self, rhs)
    return self.x * rhs.x + self.y * rhs.y + self.z * rhs.z
end

function module.vector3MetaTable.Cross(self, rhs)
    local r = {
        x = self.y * rhs.z - rhs.y * self.z,
        y = self.z * rhs.x - rhs.z * self.x,
        z = self.x * rhs.y - rhs.x * self.y
    }
    setmetatable(r, module.vector3MetaTable)
    return r
end

---- Vector 4 ----
function module.vector4MetaTable.__add(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x + rhs, y = self.y + rhs, z = self.z + rhs, w = self.w + rhs }
    else
        r = { x = self.x + rhs.x, y = self.y + rhs.y, z = self.z + rhs.z, w = self.w + rhs.w }
    end
    setmetatable(r, module.vector4MetaTable)
    return r
end

function module.vector4MetaTable.__sub(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x - rhs, y = self.y - rhs, z = self.z - rhs, w = self.w - rhs }
    else
        r = { x = self.x - rhs.x, y = self.y - rhs.y, z = self.z - rhs.z, w = self.w - rhs.w }
    end
    setmetatable(r, module.vector4MetaTable)
    return r
end

function module.vector4MetaTable.__mul(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x * rhs, y = self.y * rhs, z = self.z * rhs, w = self.w * rhs }
    else
        r = { x = self.x * rhs.x, y = self.y * rhs.y, z = self.z * rhs.z, w = self.w * rhs.w }
    end
    setmetatable(r, module.vector4MetaTable)
    return r
end

function module.vector4MetaTable.__div(self, rhs)
    local r
    if type(b) == "number" then
        rhs = 1 / rhs
        r = { x = self.x * rhs, y = self.y * rhs, z = self.z * rhs, w = self.w * rhs }
    else
        r = { x = self.x / rhs.x, y = self.y / rhs.y, z = self.z / rhs.z, w = self.w / rhs.w }
    end
    setmetatable(r, module.vector4MetaTable)
    return r
end

function module.vector4MetaTable.__mod(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x % rhs, y = self.y % rhs, z = self.z % rhs, w = self.w % rhs }
    else
        r = { x = self.x % rhs.x, y = self.y % rhs.y, z = self.z % rhs.z, w = self.w % rhs.w }
    end
    setmetatable(r, module.vector4MetaTable)
    return r
end

function module.vector4MetaTable.__pow(self, rhs)
    local r
    if type(b) == "number" then
        r = { x = self.x ^ rhs, y = self.y ^ rhs, z = self.z ^ rhs, w = self.w ^ rhs }
    else
        r = { x = self.x ^ rhs.x, y = self.y ^ rhs.y, z = self.z ^ rhs.z, w = self.w ^ rhs.w }
    end
    setmetatable(r, module.vector4MetaTable)
    return r
end

function module.vector4MetaTable.__unm(a)
    local r
    r.x = -self.x
    r.y = -self.y
    r.z = -self.z
    r.w = -self.w
    setmetatable(r, module.vector4MetaTable)
    return r
end

function module.vector4MetaTable.SquaredLength(self, rhs)
    if rhs == nil then
        return self.x*self.x + self.y*self.y + self.z*self.z + self.w*self.w
    else
        local dx = self.x - rhs.x
        local dy = self.y - rhs.y
        local dz = self.z - rhs.z
        local dw = self.w - rhs.w
        return dx*dx + dy*dy + dz*dz + dw*dw
    end
end

function module.vector4MetaTable.Length(self, rhs)
    return math.sqrt(self:SquaredLength(rhs))
end

function module.vector4MetaTable.Normalize(self)
    return self / self:Length()
end

function module.vector4MetaTable.Dot(self, rhs)
    return self.x * rhs.x + self.y * rhs.y + self.z * rhs.z + self.w * rhs.w
end

---- Constructors ----
function module.Vector2(x, y)
    local v = {}
    if type(x) == "table" then
        v = { x = x.x, y = x.y }
    elseif type(x) == "number" and type(y) == "number" then
        v = { x = x, y = y }
    else
        assert(false, "Vector2 must take 2 scalars or a table with x and y")
    end
    setmetatable(v, module.vector2MetaTable)
    return v
end

function module.Vector3(x, y, z)
    local v = {}
    if type(x) == "table" then
        v = { x = x.x, y = x.y, z = x.z }
    elseif type(x) == "number" and type(y) == "number" and type(z) == "number" then
        v = { x = x, y = y, z = z }
    else
        assert(false, "Vector3 must take 3 scalars or a table with x, y and z")
    end
    setmetatable(v, module.vector3MetaTable)
    return v
end

function module.Vector4(x, y, z, w)
    local v = {}
    if type(x) == "table" then
        v = { x = x.x, y = x.y, z = x.z, w = x.w }
    elseif type(x) == "number" and type(y) == "number" and type(z) == "number" and type(w) == "number" then
        v = { x = x, y = y, z = z, w = w }
    else
        assert(false, "Vector4 must take 4 scalars or a table with x, y, z and w")
    end
    setmetatable(v, module.vector4MetaTable)
    return v
end

---- Helpers IsVectorX() ----
function module.IsVector2(v)
    return type(v) == "table" and getmetatable(v) == module.vector2MetaTable
end

function module.IsVector3(v)
    return type(v) == "table" and getmetatable(v) == module.vector3MetaTable
end

function module.IsVector4(v)
    return type(v) == "table" and getmetatable(v) == module.vector4MetaTable
end

return module
