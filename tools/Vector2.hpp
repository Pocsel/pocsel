#if 0
#ifndef __TOOLS_VECTOR2_HPP__
#define __TOOLS_VECTOR2_HPP__

#include "tools/sqrt.hpp"

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

        static T GetDistanceSquared(glm::detail::tvec2<T> const& a, glm::detail::tvec2<T> const& b)
        {
            glm::detail::tvec2<T> delta = glm::detail::tvec2<T>(a.x - b.x, a.y - b.y);
            return delta.x*delta.x + delta.y*delta.y;
        }
        static T GetDistance(glm::detail::tvec2<T> const& a, glm::detail::tvec2<T> const& b)
        {
            return std::sqrt(GetDistanceSquared(a, b));
        }
        static bool CheckDistance(glm::detail::tvec2<T> const& a, glm::detail::tvec2<T> const& b, T distance)
        {
            return GetDistanceSquared(a, b) <= distance*distance;
        }
        static T Dot(glm::detail::tvec2<T> const& a, glm::detail::tvec2<T> const& b)
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
        template<class Tin>
        explicit Vector2(glm::detail::tvec2<Tin> const& value) : x((T)value.x), y((T)value.y)
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

        glm::detail::tvec2<T> operator +(glm::detail::tvec2<T> const& value) const
        {
            return glm::detail::tvec2<T>(this->x + value.x, this->y + value.y);
        }
        glm::detail::tvec2<T> operator -(glm::detail::tvec2<T> const& value) const
        {
            return glm::detail::tvec2<T>(this->x - value.x, this->y - value.y);
        }
        glm::detail::tvec2<T> operator -() const
        {
            return glm::detail::tvec2<T>(-this->x, -this->y);
        }
        glm::detail::tvec2<T> operator *(T value) const
        {
            return glm::detail::tvec2<T>(this->x * value, this->y * value);
        }
        glm::detail::tvec2<T> operator /(T value) const
        {
            return glm::detail::tvec2<T>(this->x / value, this->y / value);
        }
        glm::detail::tvec2<T>& operator +=(glm::detail::tvec2<T> const& value)
        {
            this->x += value.x;
            this->y += value.y;
            return *this;
        }
        glm::detail::tvec2<T>& operator -=(glm::detail::tvec2<T> const& value)
        {
            this->x -= value.x;
            this->y -= value.y;
            return *this;
        }
        glm::detail::tvec2<T>& operator *=(T value)
        {
            this->x *= value;
            this->y *= value;
            return *this;
        }
        glm::detail::tvec2<T>& operator /=(T value)
        {
            this->x /= value;
            this->y /= value;
            return *this;
        }
        bool operator ==(glm::detail::tvec2<T> const& value) const
        {
            return (this->x == value.x && this->y == value.y);
        }
        bool operator !=(glm::detail::tvec2<T> const& value) const
        {
            return (this->x != value.x || this->y != value.y);
        }
    };

    template<typename ValueType, typename VectorType>
    inline glm::detail::tvec2<VectorType> operator *(ValueType value, glm::detail::tvec2<VectorType> const& vector)
    {
        return glm::detail::tvec2<VectorType>(vector.x * value, vector.y * value);
    }

    template<typename T>
    struct Stringify<glm::detail::tvec2<T>>
    {
        static inline std::string MakeString(glm::detail::tvec2<T> const& var)
        {
            return "(" + ToString(var.x) + ", " + ToString(var.y) + ")";
        }
    };


    typedef glm::detail::tvec2<int> glm::ivec2;
    typedef glm::detail::tvec2<unsigned int> glm::uvec2;
    typedef glm::detail::tvec2<float> glm::fvec2;
    typedef glm::detail::tvec2<double> glm::dvec2;

}

#endif
#endif
