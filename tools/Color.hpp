#ifndef __TOOLS_COLOR_HPP__
#define __TOOLS_COLOR_HPP__

namespace Tools {

    template<typename T> struct Color3;
    template<typename T> Color3<T> const& ParseColor3(std::string const& s);

    template<typename T> struct Color3
    {
    public:
        union
        {
            T colors[3];
            struct { T r, g, b; };
        };

    public:
        Color3() : r(), g(), b() {}
        Color3(T r, T g, T b) : r(r), g(g), b(b) {}
        Color3(std::string const& s) { *this = ParseColor3<T>(s); }
        bool operator ==(Color3<T> const& other) const
        {
            return (
                this->r == other.r &&
                this->g == other.g &&
                this->b == other.b
            );
        }
    };

    template<typename T> struct Color4;
    template<typename T> Color4<T> const& ParseColor4(std::string const& s);

    template<typename T> struct Color4
    {
    public:
        union
        {
            T colors[4];
            struct { T r, g, b, a; };
        };

    public:
        Color4() : r(), g(), b(), a() {}
        Color4(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}
        Color4(Color3<T> const& c);
        Color4(std::string const& s) { *this = ParseColor4<T>(s); }
        bool operator ==(Color4<T> const& other) const
        {
            return (
                this->r == other.r &&
                this->g == other.g &&
                this->b == other.b &&
                this->a == other.a
            );
        }
    };

    template<typename T>
    inline Color4<T>::Color4(Color3<T> const& c) : r(c.r), g(c.g), b(c.b), a((T) -1) {}
    template<>
    inline Color4<float>::Color4(Color3<float> const& c) : r(c.r), g(c.g), b(c.b), a(1.0f) {}
    template<>
    inline Color4<double>::Color4(Color3<double> const& c) : r(c.r), g(c.g), b(c.b), a(1.0) {}

    typedef Color3<float> Color3f;
    typedef Color4<float> Color4f;

    template struct Color3<float>;
    template struct Color4<float>;

}

#endif
