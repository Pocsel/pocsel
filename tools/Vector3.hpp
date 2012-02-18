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

        static T GetDistanceSquared(Vector3<T> const& a, Vector3<T> const& b)
        {
            Vector3<T> delta = Vector3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
            return delta.x*delta.x + delta.y*delta.y + delta.z*delta.z;
        }

        static T GetDistance(Vector3<T> const& a, Vector3<T> const& b)
        {
            return std::sqrt(GetDistanceSquared(a, b));
        }
        static bool CheckDistance(Vector3<T> const& a, Vector3<T> const& b, T distance)
        {
            return GetDistanceSquared(a, b) <= distance*distance;
        }
        static T Dot(Vector3<T> const& a, Vector3<T> const& b)
        {
            return a.x*b.x + a.y*b.y + a.z*b.z;
        }
        template<class Tb, class Tout>
        static Tout Dot(Vector3<T> const& a, Vector3<Tb> const& b)
        {
            return a.x*b.x + a.y*b.y + a.z*b.z;
        }

        static Vector3<T> Cross(Vector3<T> const& a, Vector3<T> const& b)
        {
            Vector3<T> r;
            r.x = a.y * b.z - a.z * b.y;
            r.y = a.z * b.x - a.x * b.z;
            r.z = a.x * b.y - a.y * b.x;
            return r;
        }

        Vector3() : x(0), y(0), z(0)
        {
        }
        template<typename Vec2T>
        explicit Vector3(Vector2<Vec2T> const& vec2, T z = 0) : x(vec2.x), y(vec2.y), z(z)
        {
        }
        template<typename Vec3T>
        explicit Vector3(Vector3<Vec3T> const& vec3) :
            x(T(vec3.x)), y(T(vec3.y)), z(T(vec3.z))
        {
        }
        Vector3(T x, T y, T z) : x(x), y(y), z(z)
        {
        }
        explicit Vector3(T value) : x(value), y(value), z(value)
        {
        }

        Vector2<T> GetXY() const
        {
            return Tools::Vector2<T>(this->x, this->y);
        }
        template<typename Vec2T>
        Vector2<Vec2T> GetXY() const
        {
            return Tools::Vector2<Vec2T>((Vec2T)this->x, (Vec2T)this->y);
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

        Vector3<T> operator +(Vector3<T> const& value) const
        {
            return Vector3<T>(this->x + value.x, this->y + value.y, this->z + value.z);
        }
        Vector3<T> operator -(Vector3<T> const& value) const
        {
            return Vector3<T>(this->x - value.x, this->y - value.y, this->z - value.z);
        }
        Vector3<T> operator -() const
        {
            return Vector3<T>(-this->x, -this->y, -this->z);
        }
        Vector3<T> operator *(T value) const
        {
            return Vector3<T>(this->x * value, this->y * value, this->z * value);
        }
        Vector3<T> operator /(T value) const
        {
            return Vector3<T>(this->x / value, this->y / value, this->z / value);
        }
        Vector3<T>& operator +=(Vector3<T> const& value)
        {
            this->x += value.x;
            this->y += value.y;
            this->z += value.z;
            return *this;
        }
        Vector3<T>& operator -=(Vector3<T> const& value)
        {
            this->x -= value.x;
            this->y -= value.y;
            this->z -= value.z;
            return *this;
        }
        Vector3<T>& operator *=(T value)
        {
            this->x *= value;
            this->y *= value;
            this->z *= value;
            return *this;
        }
        Vector3<T>& operator /=(T value)
        {
            this->x /= value;
            this->y /= value;
            this->z /= value;
            return *this;
        }
        bool operator ==(Vector3<T> const& value) const
        {
            return (this->x == value.x && this->y == value.y && this->z == value.z);
        }
        bool operator !=(Vector3<T> const& value) const
        {
            return (this->x != value.x || this->y != value.y || this->z != value.z);
        }
    };

    template<typename ValueType, typename VectorType>
    inline Vector3<VectorType> operator *(ValueType value, Vector3<VectorType> const& vector)
    {
        return Vector3<VectorType>(vector.x * value, vector.y * value, vector.z * value);
    }

    template<typename T>
    struct Stringify<Tools::Vector3<T>>
    {
        static inline std::string MakeString(Tools::Vector3<T> const& var)
        {
            return "(" + ToString(var.x) + ", " + ToString(var.y) + ", " + ToString(var.z) + ")";
        }
    };

    typedef Vector3<int> Vector3i;
    typedef Vector3<unsigned int> Vector3u;
    typedef Vector3<float> Vector3f;
    typedef Vector3<double> Vector3d;
}

#endif
