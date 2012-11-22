local module = {}
module.matrix4MetaTable = {}

---- Matrix 4 ----
function module.matrix4MetaTable.__tostring(self)
    return
        "(" .. self.m00 .. " " .. self.m01 .. " " .. self.m02 .. " " .. self.m03 .. ")\n" ..
        "(" .. self.m10 .. " " .. self.m11 .. " " .. self.m12 .. " " .. self.m13 .. ")\n" ..
        "(" .. self.m20 .. " " .. self.m21 .. " " .. self.m22 .. " " .. self.m23 .. ")\n" ..
        "(" .. self.m30 .. " " .. self.m31 .. " " .. self.m32 .. " " .. self.m33 .. ")\n"
end

function module.matrix4MetaTable.__index(self, index)
    if index == 0 then
        return Utils.Vector4(self.m00, self.m01, self.m02, self.m03)
    elseif index == 1 then
        return Utils.Vector4(self.m10, self.m11, self.m12, self.m13)
    elseif index == 2 then
        return Utils.Vector4(self.m20, self.m21, self.m22, self.m23)
    elseif index == 3 then
        return Utils.Vector4(self.m30, self.m31, self.m32, self.m33)
    end
    return nil
end

function module.matrix4MetaTable.__newindex(self, index, value)
    assert(Utils.IsVector4(value), "value must be a Vector4")
    if index == 0 then
        self.m00 = value.x
        self.m01 = value.y
        self.m02 = value.z
        self.m03 = value.w
    elseif index == 1 then
        self.m10 = value.x
        self.m11 = value.y
        self.m12 = value.z
        self.m13 = value.w
    elseif index == 2 then
        self.m20 = value.x
        self.m21 = value.y
        self.m22 = value.z
        self.m23 = value.w
    elseif index == 3 then
        self.m30 = value.x
        self.m31 = value.y
        self.m32 = value.z
        self.m33 = value.w
    else
        assert(false, "index must be in range 0..3")
    end
end

function module.matrix4MetaTable.__div(m1, m2)
    if type(m2) == "number" then
        local r = Utils.Matrix4(m1)
        r[0] = r[0] / m2
        r[1] = r[1] / m2
        r[2] = r[2] / m2
        r[3] = r[3] / m2
        return r
    elseif Utils.IsMatrix4(m2) then
        return m1 * m2:Inverse()
    else
        assert(false, "Right operand must be a matrix4 or a number")
    end
end

function module.matrix4MetaTable.__mul(m1, m2)
    local m1m00 = m1.m00
    local m1m01 = m1.m01
    local m1m02 = m1.m02
    local m1m03 = m1.m03
    local m1m10 = m1.m10
    local m1m11 = m1.m11
    local m1m12 = m1.m12
    local m1m13 = m1.m13
    local m1m20 = m1.m20
    local m1m21 = m1.m21
    local m1m22 = m1.m22
    local m1m23 = m1.m23
    local m1m30 = m1.m30
    local m1m31 = m1.m31
    local m1m32 = m1.m32
    local m1m33 = m1.m33

    local m2m00 = m2.m00
    local m2m01 = m2.m01
    local m2m02 = m2.m02
    local m2m03 = m2.m03
    local m2m10 = m2.m10
    local m2m11 = m2.m11
    local m2m12 = m2.m12
    local m2m13 = m2.m13
    local m2m20 = m2.m20
    local m2m21 = m2.m21
    local m2m22 = m2.m22
    local m2m23 = m2.m23
    local m2m30 = m2.m30
    local m2m31 = m2.m31
    local m2m32 = m2.m32
    local m2m33 = m2.m33

    r = {
        m00 = m1m00 * m2m00 + m1m01 * m2m10 + m1m02 * m2m20 + m1m03 * m2m30,
        m01 = m1m00 * m2m01 + m1m01 * m2m11 + m1m02 * m2m21 + m1m03 * m2m31,
        m02 = m1m00 * m2m02 + m1m01 * m2m12 + m1m02 * m2m22 + m1m03 * m2m32,
        m03 = m1m00 * m2m03 + m1m01 * m2m13 + m1m02 * m2m23 + m1m03 * m2m33,
        m10 = m1m10 * m2m00 + m1m11 * m2m10 + m1m12 * m2m20 + m1m13 * m2m30,
        m11 = m1m10 * m2m01 + m1m11 * m2m11 + m1m12 * m2m21 + m1m13 * m2m31,
        m12 = m1m10 * m2m02 + m1m11 * m2m12 + m1m12 * m2m22 + m1m13 * m2m32,
        m13 = m1m10 * m2m03 + m1m11 * m2m13 + m1m12 * m2m23 + m1m13 * m2m33,
        m20 = m1m20 * m2m00 + m1m21 * m2m10 + m1m22 * m2m20 + m1m23 * m2m30,
        m21 = m1m20 * m2m01 + m1m21 * m2m11 + m1m22 * m2m21 + m1m23 * m2m31,
        m22 = m1m20 * m2m02 + m1m21 * m2m12 + m1m22 * m2m22 + m1m23 * m2m32,
        m23 = m1m20 * m2m03 + m1m21 * m2m13 + m1m22 * m2m23 + m1m23 * m2m33,
        m30 = m1m30 * m2m00 + m1m31 * m2m10 + m1m32 * m2m20 + m1m33 * m2m30,
        m31 = m1m30 * m2m01 + m1m31 * m2m11 + m1m32 * m2m21 + m1m33 * m2m31,
        m32 = m1m30 * m2m02 + m1m31 * m2m12 + m1m32 * m2m22 + m1m33 * m2m32,
        m33 = m1m30 * m2m03 + m1m31 * m2m13 + m1m32 * m2m23 + m1m33 * m2m33,
    }
    setmetatable(r, module.matrix4MetaTable)
    return r
end

function module.matrix4MetaTable.Inverse(self)
    local Coef00 = self.m22 * self.m33 - self.m32 * self.m23
    local Coef02 = self.m12 * self.m33 - self.m32 * self.m13
    local Coef03 = self.m12 * self.m23 - self.m22 * self.m13
    local Coef04 = self.m21 * self.m33 - self.m31 * self.m23
    local Coef06 = self.m11 * self.m33 - self.m31 * self.m13
    local Coef07 = self.m11 * self.m23 - self.m21 * self.m13
    local Coef08 = self.m21 * self.m32 - self.m31 * self.m22
    local Coef10 = self.m11 * self.m32 - self.m31 * self.m12
    local Coef11 = self.m11 * self.m22 - self.m21 * self.m12
    local Coef12 = self.m20 * self.m33 - self.m30 * self.m23
    local Coef14 = self.m10 * self.m33 - self.m30 * self.m13
    local Coef15 = self.m10 * self.m23 - self.m20 * self.m13
    local Coef16 = self.m20 * self.m32 - self.m30 * self.m22
    local Coef18 = self.m10 * self.m32 - self.m30 * self.m12
    local Coef19 = self.m10 * self.m22 - self.m20 * self.m12
    local Coef20 = self.m20 * self.m31 - self.m30 * self.m21
    local Coef22 = self.m10 * self.m31 - self.m30 * self.m11
    local Coef23 = self.m10 * self.m21 - self.m20 * self.m11

    local SignA = Utils.Vector4( 1, -1,  1, -1)
    local SignB = Utils.Vector4(-1,  1, -1,  1)

    local Fac0 = Utils.Vector4(Coef00, Coef00, Coef02, Coef03)
    local Fac1 = Utils.Vector4(Coef04, Coef04, Coef06, Coef07)
    local Fac2 = Utils.Vector4(Coef08, Coef08, Coef10, Coef11)
    local Fac3 = Utils.Vector4(Coef12, Coef12, Coef14, Coef15)
    local Fac4 = Utils.Vector4(Coef16, Coef16, Coef18, Coef19)
    local Fac5 = Utils.Vector4(Coef20, Coef20, Coef22, Coef23)

    local Vec0 = Utils.Vector4(self.m10, self.m00, self.m00, self.m00)
    local Vec1 = Utils.Vector4(self.m11, self.m01, self.m01, self.m01)
    local Vec2 = Utils.Vector4(self.m12, self.m02, self.m02, self.m02)
    local Vec3 = Utils.Vector4(self.m13, self.m03, self.m03, self.m03)

    local Inv0 = SignA * (Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2)
    local Inv1 = SignB * (Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4)
    local Inv2 = SignA * (Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5)
    local Inv3 = SignB * (Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5)

    local Inverse = Utils.Matrix4(Inv0, Inv1, Inv2, Inv3)

    local Row0 = Utils.Vector4(Inverse.m00, Inverse.m10, Inverse.m20, Inverse.m30)

    local Determinant = self[0]:Dot(Row0);

    return Inverse / Determinant
end

---- Constructors ----
function Utils.Matrix4(
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33)
    local m = {}
    if not m00 then
        m = {
            m00 = 1, m01 = 0, m02 = 0, m03 = 0,
            m10 = 0, m11 = 1, m12 = 0, m13 = 0,
            m20 = 0, m21 = 0, m22 = 1, m23 = 0,
            m30 = 0, m31 = 0, m32 = 0, m33 = 1
        }
    elseif Utils.IsMatrix4(m00) then
        m = {
            m00 = m00.m00, m01 = m00.m01, m02 = m00.m02, m03 = m00.m03,
            m10 = m00.m10, m11 = m00.m11, m12 = m00.m12, m13 = m00.m13,
            m20 = m00.m20, m21 = m00.m21, m22 = m00.m22, m23 = m00.m23,
            m30 = m00.m30, m31 = m00.m31, m32 = m00.m32, m33 = m00.m33
        }
    elseif Utils.IsVector4(m00) and Utils.IsVector4(m01) and Utils.IsVector4(m02) and Utils.IsVector4(m03) then
        m = {
            m00 = m00.x, m01 = m00.y, m02 = m00.z, m03 = m00.w,
            m10 = m01.x, m11 = m01.y, m12 = m01.z, m13 = m01.w,
            m20 = m02.x, m21 = m02.y, m22 = m02.z, m23 = m02.w,
            m30 = m03.x, m31 = m03.y, m32 = m03.z, m33 = m03.w
        }
    elseif type(m00) == "number" and not m01 then
        m = {
            m00 = m00, m01 = 0, m02 = 0, m03 = 0,
            m10 = 0, m11 = m00, m12 = 0, m13 = 0,
            m20 = 0, m21 = 0, m22 = m00, m23 = 0,
            m30 = 0, m31 = 0, m32 = 0, m33 = m00
        }
    else
        m = {
            m00 = m00, m01 = m01, m02 = m02, m03 = m03,
            m10 = m10, m11 = m11, m12 = m12, m13 = m13,
            m20 = m20, m21 = m21, m22 = m22, m23 = m23,
            m30 = m30, m31 = m31, m32 = m32, m33 = m33
        }
    end
    setmetatable(m, module.matrix4MetaTable)
    return m
end

---- Helpers IsMatrix4() ----
function Utils.IsMatrix4(m)
    return type(m) == "table" and getmetatable(m) == module.matrix4MetaTable
end

return module
