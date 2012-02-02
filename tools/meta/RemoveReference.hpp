#ifndef __TOOLS_META_REMOVEREFERENCE_HPP__
#define __TOOLS_META_REMOVEREFERENCE_HPP__

namespace Tools { namespace Meta {

    template<typename T>
    struct RemoveReference
    {
        typedef T Result;
    };

    template<typename T>
    struct RemoveReference<T&>
    {
        typedef T Result;
    };

}}

#endif
