#if 0
#ifndef __TOOLS_VECTOR3_HPP__
#define __TOOLS_VECTOR3_HPP__

#include "tools/sqrt.hpp"
#include "tools/Vector2.hpp"

namespace Tools {

    template<typename T>
    struct Vector3
    {
        union
        {
            T coords[3];
            struct { T x, y, z; };
            struct { T u, v, w; };
        };

        static T GetDistanceSquared(glm::detail::tvec3<T> const& a, glm::detail::tvec3<T> const& b)
        {
            glm::detail::tvec3<T> delta = glm::detail::tvec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
            return delta.x*delta.x + delta.y*delta.y + delta.z*delta.z;
        }

        static T GetDistance(glm::detail::tvec3<T> const& a, glm::detail::tvec3<T> const& b)
        {
            return std::sqrt(GetDistanceSquared(a, b));
        }
        static bool CheckDistance(glm::detail::tvec3<T> const& a, glm::detail::tvec3<T> const& b, T distance)
        {
            return GetDistanceSquared(a, b) <= distance*distance;
        }
        static T Dot(glm::detail::tvec3<T> const& a, glm::detail::tvec3<T> const& b)
        {
            return a.x*b.x + a.y*b.y + a.z*b.z;
        }
        template<class Tb, class Tout>
        static Tout Dot(glm::detail::tvec3<T> const& a, glm::detail::tvec3<Tb> const& b)
        {
            return a.x*b.x + a.y*b.y + a.z*b.z;
        }

        static glm::detail::tvec3<T> Cross(glm::detail::tvec3<T> const& a, glm::detail::tvec3<T> const& b)
        {
            glm::detail::tvec3<T> r;
            r.x = a.y * b.z - a.z * b.y;
            r.y = a.z * b.x - a.x * b.z;
            r.z = a.x * b.y - a.y * b.x;
            return r;
        }

        Vector3() : x(0), y(0), z(0)
        {
        }
        template<typename Vec2T>
        explicit Vector3(glm::detail::tvec2<Vec2T> const& vec2, T z = 0) : x(vec2.x), y(vec2.y), z(z)
        {
        }
        template<typename Vec3T>
        explicit Vector3(glm::detail::tvec3<Vec3T> const& vec3) :
            x(T(vec3.x)), y(T(vec3.y)), z(T(vec3.z))
        {
        }
        Vector3(T x, T y, T z) : x(x), y(y), z(z)
        {
        }
        explicit Vector3(T value) : x(value), y(value), z(value)
        {
        }

        glm::detail::tvec2<T> GetXY() const
        {
            return glm::detail::tvec2<T>(this->x, this->y);
        }
        template<typename Vec2T>
        glm::detail::tvec2<Vec2T> GetXY() const
        {
            return glm::detail::tvec2<Vec2T>((Vec2T)this->x, (Vec2T)this->y);
        }
        T GetMagnitude() const
        {
            return std::sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
        }

        T GetMagnitudeSquared() const
        {
            return this->x*this->x + this->y*this->y + this->z*this->z;
        }

        void Normalize()
        {
            T mag = this->GetMagnitude();
            if (mag > 0)
                *this /= mag;
            else
            {
                this->x = 0;
                this->y = 0;
                this->z = 0;
            }
        }

        glm::detail::tvec3<T> operator +(glm::detail::tvec3<T> const& value) const
        {
            return glm::detail::tvec3<T>(this->x + value.x, this->y + value.y, this->z + value.z);
        }
        glm::detail::tvec3<T> operator -(glm::detail::tvec3<T> const& value) const
        {
            return glm::detail::tvec3<T>(this->x - value.x, this->y - value.y, this->z - value.z);
        }
        glm::detail::tvec3<T> operator -() const
        {
            return glm::detail::tvec3<T>(-this->x, -this->y, -this->z);
        }
        glm::detail::tvec3<T> operator *(T value) const
        {
            return glm::detail::tvec3<T>(this->x * value, this->y * value, this->z * value);
        }
        glm::detail::tvec3<T> operator /(T value) const
        {
            return glm::detail::tvec3<T>(this->x / value, this->y / value, this->z / value);
        }
        glm::detail::tvec3<T>& operator +=(glm::detail::tvec3<T> const& value)
        {
            this->x += value.x;
            this->y += value.y;
            this->z += value.z;
            return *this;
        }
        glm::detail::tvec3<T>& operator -=(glm::detail::tvec3<T> const& value)
        {
            this->x -= value.x;
            this->y -= value.y;
            this->z -= value.z;
            return *this;
        }
        glm::detail::tvec3<T>& operator *=(T value)
        {
            this->x *= value;
            this->y *= value;
            this->z *= value;
            return *this;
        }
        glm::detail::tvec3<T>& operator /=(T value)
        {
            this->x /= value;
            this->y /= value;
            this->z /= value;
            return *this;
        }
        bool operator ==(glm::detail::tvec3<T> const& value) const
        {
            return (this->x == value.x && this->y == value.y && this->z == value.z);
        }
        bool operator !=(glm::detail::tvec3<T> const& value) const
        {
            return (this->x != value.x || this->y != value.y || this->z != value.z);
        }
    };

    template<typename ValueType, typename VectorType>
    inline glm::detail::tvec3<VectorType> operator *(ValueType value, glm::detail::tvec3<VectorType> const& vector)
    {
        return glm::detail::tvec3<VectorType>(vector.x * value, vector.y * value, vector.z * value);
    }

    template<typename T>
    struct Stringify<glm::detail::tvec3<T>>
    {
        static inline std::string MakeString(glm::detail::tvec3<T> const& var)
        {
            return "(" + ToString(var.x) + ", " + ToString(var.y) + ", " + ToString(var.z) + ")";
        }
    };

    typedef glm::detail::tvec3<int> glm::ivec3;
    typedef glm::detail::tvec3<unsigned int> glm::uvec3;
    typedef glm::detail::tvec3<float> glm::fvec3;
    typedef glm::detail::tvec3<double> glm::dvec3;

}

#endif
#endif
