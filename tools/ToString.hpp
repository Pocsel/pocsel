#ifndef __TOOLS_TOSTRING_HPP__
#define __TOOLS_TOSTRING_HPP__

#define HAS_TOSTRING

namespace Tools {

    template<typename T> struct Stringify
    {
        static std::string MakeString(T const& val)
        {
            std::stringstream ss;
            ss << val;
            return ss.str();
        }
    };

    template<typename T> struct Stringify<T*>
    {
        static std::string MakeString(T const* val)
        {
            std::stringstream ss;
            ss << std::hex << val;
            return ss.str();
        }
    };

    template<> struct Stringify<char const*>
    {
        static std::string MakeString(char const* val)
        {
            static std::string nil("(nil)");
            if (val != 0)
                return std::string(val);
            return nil;
        }
    };

    template<> struct Stringify<bool>
    {
        static std::string MakeString(bool const& val)
        {
            static std::string const trueString("true"), falseString("false");
            return (val ? trueString : falseString);
        }
    };

    template<> struct Stringify<Int8>
    {
        static std::string MakeString(Int8 val)
        {
            return Stringify<Int16>::MakeString(val);
        }
    };

    template<> struct Stringify<Uint8>
    {
        static std::string MakeString(Uint8 val)
        {
            return Stringify<Uint16>::MakeString(val);
        }
    };

    template<> struct Stringify<std::string>
    {
        static std::string MakeString(std::string const& val)
        {
            return val;
        }
    };

    // the function
    template<typename T> inline std::string ToString(T val)
    {
        return Stringify<T>::MakeString(val);
    }

}

#endif
