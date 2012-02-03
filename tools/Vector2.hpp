#ifndef __TOOLS_VECTOR2_HPP__
#define __TOOLS_VECTOR2_HPP__

#include <cmath>
#include "tools/sqrt.hpp"
#include "tools/ToString.hpp"

namespace Tools {

    template<typename T>
    struct Vector2
    {
        union
        {
            T coords[2];
            struct { T x, y; };
            struct { T u, v; };
            struct { T w, h; };
        };

        static T GetDistanceSquared(Vector2<T> const& a, Vector2<T> const& b)
        {
            Vector2<T> delta = Vector2<T>(a.x - b.x, a.y - b.y);
            return delta.x*delta.x + delta.y*delta.y;
        }
        static T GetDistance(Vector2<T> const& a, Vector2<T> const& b)
        {
            return std::sqrt(GetDistanceSquared(a, b));
        }
        static bool CheckDistance(Vector2<T> const& a, Vector2<T> const& b, T distance)
        {
            return GetDistanceSquared(a, b) <= distance*distance;
        }
        static T Dot(Vector2<T> const& a, Vector2<T> const& b)
        {
            return a.x*b.x + a.y*b.y;
        }

        Vector2() : x(0), y(0)
        {
        }
        Vector2(T x, T y) : x(x), y(y)
        {
        }
        explicit Vector2(T value) : x(value), y(value)
        {
        }

        T GetMagnitude() const
        {
            return std::sqrt(this->x*this->x + this->y*this->y);
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
            }
        }

        Vector2<T> operator +(Vector2<T> const& value) const
        {
            return Vector2<T>(this->x + value.x, this->y + value.y);
        }
        Vector2<T> operator -(Vector2<T> const& value) const
        {
            return Vector2<T>(this->x - value.x, this->y - value.y);
        }
        Vector2<T> operator -() const
        {
            return Vector2<T>(-this->x, -this->y);
        }
        Vector2<T> operator *(T value) const
        {
            return Vector2<T>(this->x * value, this->y * value);
        }
        Vector2<T> operator /(T value) const
        {
            return Vector2<T>(this->x / value, this->y / value);
        }
        Vector2<T>& operator +=(Vector2<T> const& value)
        {
            this->x += value.x;
            this->y += value.y;
            return *this;
        }
        Vector2<T>& operator -=(Vector2<T> const& value)
        {
            this->x -= value.x;
            this->y -= value.y;
            return *this;
        }
        Vector2<T>& operator *=(T value)
        {
            this->x *= value;
            this->y *= value;
            return *this;
        }
        Vector2<T>& operator /=(T value)
        {
            this->x /= value;
            this->y /= value;
            return *this;
        }
        bool operator ==(Vector2<T> const& value) const
        {
            return (this->x == value.x && this->y == value.y);
        }
        bool operator !=(Vector2<T> const& value) const
        {
            return (this->x != value.x || this->y != value.y);
        }
    };

    template<typename ValueType, typename VectorType>
    inline Vector2<VectorType> operator *(ValueType value, Vector2<VectorType> const& vector)
    {
        return Vector2<VectorType>(vector.x * value, vector.y * value);
    }

    template<typename T>
    struct Stringify<Tools::Vector2<T>>
    {
        static inline std::string MakeString(Tools::Vector2<T> const& var)
        {
            return "(" + ToString(var.x) + ", " + ToString(var.y) + ")";
        }
    };


    typedef Tools::Vector2<int> Vector2i;
    typedef Tools::Vector2<unsigned int> Vector2u;
    typedef Tools::Vector2<float> Vector2f;
    typedef Tools::Vector2<double> Vector2d;

}

#endif
