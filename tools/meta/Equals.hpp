#ifndef __TOOLS_META_EQUALS_HPP__
#define __TOOLS_META_EQUALS_HPP__

namespace Tools { namespace Meta {

    template<typename T, typename U> struct Equals
    {
        static bool const result = false;
    };
    template<typename T> struct Equals<T, T>
    {
        static bool const result = true;
    };

}}

#endif
