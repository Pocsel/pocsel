#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <cassert>
#include <boost/algorithm/string.hpp>

#include "tools/Color.hpp"

namespace {

    template<typename T> struct Coef
    {
        static T Convert(float percent)
        {
            assert(percent >= 0 && percent <= 1 && "Wrong percent number");
            return static_cast<T>(
                    static_cast<double>(1L << (sizeof(T) * 8)) * static_cast<double>(percent)
                    );
        }
    };
    template<> struct Coef<float> { static float Convert(float p) { return p; } };
    template<> struct Coef<double> { static double Convert(double p) { return p; } };

# define COLOR3(r, g, b) \
    Tools::Color3<T>(Coef<T>::Convert(r), Coef<T>::Convert(g), Coef<T>::Convert(b))
    template<typename T>
        std::unordered_map<std::string, Tools::Color3<T>>& InitColor3Map()
        {
            static std::unordered_map<std::string, Tools::Color3<T>> map;
            static bool called = false;
            if (called)
                return map;
            called = true;
            map["aliceblue"] =           COLOR3(0.94f,  0.97f,  1.0f);
            map["antiquewhite"] =        COLOR3(0.98f,  0.92f,  0.84f);
            map["aqua"] =                COLOR3(0.0f,   1.0f,   1.0f);
            map["aquamarine"] =          COLOR3(0.50f,  1.0f,   0.83f);
            map["azure"] =               COLOR3(0.94f,  1.0f,   1.0f);
            map["beige"] =               COLOR3(0.96f,  0.96f,  0.86f);
            map["bisque"] =              COLOR3(1.0f,   0.89f,  0.77f);
            map["black"] =               COLOR3(0.0f,   0.0f,   0.0f);
            map["blanchedalmond"] =      COLOR3(1.0f,   0.92f,  0.80f);
            map["blue"] =                COLOR3(0.0f,   0.0f,   1.0f);
            map["blueviolet"] =          COLOR3(0.54f,  0.17f,  0.89f);
            map["brown"] =               COLOR3(0.65f,  0.16f,  0.16f);
            map["burlywood"] =           COLOR3(0.87f,  0.72f,  0.53f);
            map["cadetblue"] =           COLOR3(0.37f,  0.62f,  0.63f);
            map["chartreuse"] =          COLOR3(0.50f,  1.0f,   0.0f);
            map["chocolate"] =           COLOR3(0.82f,  0.41f,  0.12f);
            map["coral"] =               COLOR3(1.0f,   0.50f,  0.31f);
            map["cornflower"] =          COLOR3(0.39f,  0.58f,  0.93f);
            map["cornsilk"] =            COLOR3(1.0f,   0.97f,  0.86f);
            map["crimson"] =             COLOR3(0.86f,  0.8f,   0.24f);
            map["cyan"] =                COLOR3(0.0f,   1.0f,   1.0f);
            map["darkblue"] =            COLOR3(0.0f,   0.0f,   0.55f);
            map["darkcyan"] =            COLOR3(0.0f,   0.55f,  0.55f);
            map["darkgoldenrod"] =       COLOR3(0.72f,  0.53f,  0.4f);
            map["darkgray"] =            COLOR3(0.66f,  0.66f,  0.66f);
            map["darkgreen"] =           COLOR3(0.0f,   0.39f,  0.0f);
            map["darkkhaki"] =           COLOR3(0.74f,  0.72f,  0.42f);
            map["darkmagenta"] =         COLOR3(0.55f,  0.0f,   0.55f);
            map["darkolivegreen"] =      COLOR3(0.33f,  0.42f,  0.18f);
            map["darkorange"] =          COLOR3(1.0f,   0.55f,  0.0f);
            map["darkorchid"] =          COLOR3(0.60f,  0.20f,  0.80f);
            map["darkred"] =             COLOR3(0.55f,  0.0f,   0.0f);
            map["darksalmon"] =          COLOR3(0.91f,  0.59f,  0.48f);
            map["darkseagreen"] =        COLOR3(0.56f,  0.74f,  0.56f);
            map["darkslateblue"] =       COLOR3(0.28f,  0.24f,  0.55f);
            map["darkslategray"] =       COLOR3(0.18f,  0.31f,  0.31f);
            map["darkturquoise"] =       COLOR3(0.0f,   0.81f,  0.82f);
            map["darkviolet"] =          COLOR3(0.58f,  0.0f,   0.83f);
            map["deeppink"] =            COLOR3(1.0f,   0.8f,   0.58f);
            map["deepskyblue"] =         COLOR3(0.0f,   0.75f,  1.0f);
            map["dimgray"] =             COLOR3(0.41f,  0.41f,  0.41f);
            map["dodgerblue"] =          COLOR3(0.12f,  0.56f,  1.0f);
            map["firebrick"] =           COLOR3(0.70f,  0.13f,  0.13f);
            map["floralwhite"] =         COLOR3(1.0f,   0.98f,  0.94f);
            map["forestgreen"] =         COLOR3(0.13f,  0.55f,  0.13f);
            map["fuchsia"] =             COLOR3(1.0f,   0.0f,   1.0f);
            map["gainsboro"] =           COLOR3(0.86f,  0.86f,  0.86f);
            map["ghostwhite"] =          COLOR3(0.97f,  0.97f,  1.0f);
            map["gold"] =                COLOR3(1.0f,   0.84f,  0.0f);
            map["goldenrod"] =           COLOR3(0.85f,  0.65f,  0.13f);
            map["gray"] =                COLOR3(0.75f,  0.75f,  0.75f);
            map["green"] =               COLOR3(0.0f,   1.0f,   0.0f);
            map["greenyellow"] =         COLOR3(0.68f,  1.0f,   0.18f);
            map["honeydew"] =            COLOR3(0.94f,  1.0f,   0.94f);
            map["hotpink"] =             COLOR3(1.0f,   0.41f,  0.71f);
            map["indianred"] =           COLOR3(0.80f,  0.36f,  0.36f);
            map["indigo"] =              COLOR3(0.29f,  0.0f,   0.51f);
            map["ivory"] =               COLOR3(1.0f,   1.0f,   0.94f);
            map["khaki"] =               COLOR3(0.94f,  0.90f,  0.55f);
            map["lavender"] =            COLOR3(0.90f,  0.90f,  0.98f);
            map["lavenderblush"] =       COLOR3(1.0f,   0.94f,  0.96f);
            map["lawngreen"] =           COLOR3(0.49f,  0.99f,  0.0f);
            map["lemonchiffon"] =        COLOR3(1.0f,   0.98f,  0.80f);
            map["lightblue"] =           COLOR3(0.68f,  0.85f,  0.90f);
            map["lightcoral"] =          COLOR3(0.94f,  0.50f,  0.50f);
            map["lightcyan"] =           COLOR3(0.88f,  1.0f,   1.0f);
            map["lightgoldenrod"] =      COLOR3(0.98f,  0.98f,  0.82f);
            map["lightgray"] =           COLOR3(0.83f,  0.83f,  0.83f);
            map["lightgreen"] =          COLOR3(0.56f,  0.93f,  0.56f);
            map["lightpink"] =           COLOR3(1.0f,   0.71f,  0.76f);
            map["lightsalmon"] =         COLOR3(1.0f,   0.63f,  0.48f);
            map["lightseagreen"] =       COLOR3(0.13f,  0.70f,  0.67f);
            map["lightskyblue"] =        COLOR3(0.53f,  0.81f,  0.98f);
            map["lightslategray"] =      COLOR3(0.47f,  0.53f,  0.60f);
            map["lightsteelblue"] =      COLOR3(0.69f,  0.77f,  0.87f);
            map["lightyellow"] =         COLOR3(1.0f,   1.0f,   0.88f);
            map["lime"] =                COLOR3(0.0f,   1.0f,   0.0f);
            map["limegreen"] =           COLOR3(0.20f,  0.80f,  0.20f);
            map["linen"] =               COLOR3(0.98f,  0.94f,  0.90f);
            map["magenta"] =             COLOR3(1.0f,   0.0f,   1.0f);
            map["maroon"] =              COLOR3(0.69f,  0.19f,  0.38f);
            map["mediumaquamarine"] =    COLOR3(0.40f,  0.80f,  0.67f);
            map["mediumblue"] =          COLOR3(0.0f,   0.0f,   0.80f);
            map["mediumorchid"] =        COLOR3(0.73f,  0.33f,  0.83f);
            map["mediumpurple"] =        COLOR3(0.58f,  0.44f,  0.86f);
            map["mediumseagreen"] =      COLOR3(0.24f,  0.70f,  0.44f);
            map["mediumslateblue"] =     COLOR3(0.48f,  0.41f,  0.93f);
            map["mediumspringgreen"] =   COLOR3(0.0f,   0.98f,  0.60f);
            map["mediumturquoise"] =     COLOR3(0.28f,  0.82f,  0.80f);
            map["mediumvioletred"] =     COLOR3(0.78f,  0.8f,   0.52f);
            map["midnightblue"] =        COLOR3(0.10f,  0.10f,  0.44f);
            map["mintcream"] =           COLOR3(0.96f,  1.0f,   0.98f);
            map["mistyrose"] =           COLOR3(1.0f,   0.89f,  0.88f);
            map["moccasin"] =            COLOR3(1.0f,   0.89f,  0.71f);
            map["navajowhite"] =         COLOR3(1.0f,   0.87f,  0.68f);
            map["navy"] =                COLOR3(0.0f,   0.0f,   0.50f);
            map["oldlace"] =             COLOR3(0.99f,  0.96f,  0.90f);
            map["olive"] =               COLOR3(0.50f,  0.50f,  0.0f);
            map["olivedrab"] =           COLOR3(0.42f,  0.56f,  0.14f);
            map["orange"] =              COLOR3(1.0f,   0.65f,  0.0f);
            map["orangered"] =           COLOR3(1.0f,   0.27f,  0.0f);
            map["orchid"] =              COLOR3(0.85f,  0.44f,  0.84f);
            map["palegoldenrod"] =       COLOR3(0.93f,  0.91f,  0.67f);
            map["palegreen"] =           COLOR3(0.60f,  0.98f,  0.60f);
            map["paleturquoise"] =       COLOR3(0.69f,  0.93f,  0.93f);
            map["palevioletred"] =       COLOR3(0.86f,  0.44f,  0.58f);
            map["papayawhip"] =          COLOR3(1.0f,   0.94f,  0.84f);
            map["peachpuff"] =           COLOR3(1.0f,   0.85f,  0.73f);
            map["peru"] =                COLOR3(0.80f,  0.52f,  0.25f);
            map["pink"] =                COLOR3(1.0f,   0.75f,  0.80f);
            map["plum"] =                COLOR3(0.87f,  0.63f,  0.87f);
            map["powderblue"] =          COLOR3(0.69f,  0.88f,  0.90f);
            map["purple"] =              COLOR3(0.63f,  0.13f,  0.94f);
            map["red"] =                 COLOR3(1.0f,   0.0f,   0.0f);
            map["rosybrown"] =           COLOR3(0.74f,  0.56f,  0.56f);
            map["royalblue"] =           COLOR3(0.25f,  0.41f,  0.88f);
            map["saddlebrown"] =         COLOR3(0.55f,  0.27f,  0.7f);
            map["salmon"] =              COLOR3(0.98f,  0.50f,  0.45f);
            map["sandybrown"] =          COLOR3(0.96f,  0.64f,  0.38f);
            map["seagreen"] =            COLOR3(0.18f,  0.55f,  0.34f);
            map["seashell"] =            COLOR3(1.0f,   0.96f,  0.93f);
            map["sienna"] =              COLOR3(0.63f,  0.32f,  0.18f);
            map["silver"] =              COLOR3(0.75f,  0.75f,  0.75f);
            map["skyblue"] =             COLOR3(0.53f,  0.81f,  0.92f);
            map["slateblue"] =           COLOR3(0.42f,  0.35f,  0.80f);
            map["slategray"] =           COLOR3(0.44f,  0.50f,  0.56f);
            map["snow"] =                COLOR3(1.0f,   0.98f,  0.98f);
            map["springgreen"] =         COLOR3(0.0f,   1.0f,   0.50f);
            map["steelblue"] =           COLOR3(0.27f,  0.51f,  0.71f);
            map["tan"] =                 COLOR3(0.82f,  0.71f,  0.55f);
            map["teal"] =                COLOR3(0.0f,   0.50f,  0.50f);
            map["thistle"] =             COLOR3(0.85f,  0.75f,  0.85f);
            map["tomato"] =              COLOR3(1.0f,   0.39f,  0.28f);
            map["turquoise"] =           COLOR3(0.25f,  0.88f,  0.82f);
            map["violet"] =              COLOR3(0.93f,  0.51f,  0.93f);
            map["wheat"] =               COLOR3(0.96f,  0.87f,  0.70f);
            map["white"] =               COLOR3(1.0f,   1.0f,   1.0f);
            map["whitesmoke"] =          COLOR3(0.96f,  0.96f,  0.96f);
            map["yellow"] =              COLOR3(1.0f,   1.0f,   0.0f);
            map["yellowgreen"] =         COLOR3(0.60f,  0.80f,  0.20f);
            return map;
        }
# undef COLOR3

# define COLOR4(r, g, b, a) \
    Tools::Color4<T>(Coef<T>::Convert(r), Coef<T>::Convert(g), Coef<T>::Convert(b), Coef<T>::Convert(a))
    template<typename T>
        std::unordered_map<std::string, Tools::Color4<T>>& InitColor4Map()
        {
            static std::unordered_map<std::string, Tools::Color4<T>> map;
            static bool called = false;
            if (called)
                return map;
            called = true;
            auto color3_map = InitColor3Map<float>();
            auto it = color3_map.begin(),
                 end = color3_map.end();
            for (; it != end; ++it)
            {
                Tools::Color3<float> const& c = it->second;
                map[it->first] = COLOR4(c.r, c.g, c.b, 1.0f);
            }
            map["transparent"] =  COLOR4(1.0f, 1.0f, 1.0f, 0.0f);
            return map;
        }
# undef COLOR4

    template<typename T>
    inline void FillHexColors(char const* str, size_t len, T* colors, size_t n)
    {
        if (len < n || len % n != 0)
        {
            std::cerr << "Wrong string length " << len << " for " << n << " colors components in \"" << str << "\"." << std::endl;
            return;
        }
        unsigned int chars_by_colors = len / n;
        unsigned int max_val = 1 << (4 * chars_by_colors);
        for (size_t col_idx = 0; col_idx < n; ++col_idx)
        {
            unsigned int col = 0;
            for (unsigned int char_idx = 0; char_idx < chars_by_colors; ++char_idx)
            {
                char c = str[char_idx];
                if (c >= '0' && c <= '9')
                    col = col * 16 + (c - '0');
                else if (c >= 'a' && c <= 'f')
                    col = col * 16 + (c - 'a' + 10);
                else if (c >= 'A' && c <= 'F')
                    col = col * 16 + (c - 'A' + 10);
                else
                {
                    std::cerr << "Wrong char " << c << " in string \"" << str << "\"." << std::endl;
                    return;
                }
            }
            colors[col_idx] = Coef<T>::Convert(static_cast<float>(col) / static_cast<float>(max_val));
            str += chars_by_colors;
        }
    }

} // !anonymous

namespace Tools {

    template<typename T>
    Color3<T> const& ParseColor3(std::string const& s)
    {
        static std::unordered_map<std::string, Color3<T>>& map = InitColor3Map<T>();
        static Color3<T> black(
            Coef<T>::Convert(0),
            Coef<T>::Convert(0),
            Coef<T>::Convert(0)
        );
        if (s.size() == 0)
            return black;

        std::string ls = s;
        boost::to_lower(ls);
        auto it = map.find(ls);
        if (it == map.end())
        {
            if (s[0] == '#')
            {
                Color3<T>& c = map[ls];
                FillHexColors(ls.c_str() + 1, ls.size() - 1, c.colors, 3);
                return c;
            }
            return black;
        }
        return it->second;
    }

    template<typename T>
    Color4<T> const& ParseColor4(std::string const& s)
    {
        static std::unordered_map<std::string, Color4<T>>& map = InitColor4Map<T>();
        static Color4<T> black(
            Coef<T>::Convert(0.0f),
            Coef<T>::Convert(0.0f),
            Coef<T>::Convert(0.0f),
            Coef<T>::Convert(1.0f)
        );
        if (s.size() == 0)
            return black;

        std::string ls = s;
        boost::to_lower(ls);
        auto it = map.find(ls);
        if (it == map.end())
        {
            if (s[0] == '#')
            {
                Color4<T>& c = map[ls];
                auto str_size = ls.size() - 1;
                if (str_size % 4 == 0)
                {
                    FillHexColors(ls.c_str() + 1, str_size, c.colors, 4);
                }
                else if (str_size % 3 == 0)
                {
                    FillHexColors(ls.c_str() + 1, str_size, c.colors, 3);
                    c.a = Coef<T>::Convert(1.0f);
                }
                return c;
            }
            return black;
        }
        return it->second;
    }

    template Color4<float> const& ParseColor4(std::string const&);
    template Color3<float> const& ParseColor3(std::string const&);

}
