#ifndef __TOOLS_META_REMOVEQUALIFIERS_HPP__
#define __TOOLS_META_REMOVEQUALIFIERS_HPP__

namespace Tools { namespace Meta {

    template<typename T>
    struct RemoveConst
    {
        typedef T Result;
    };

    template<typename T>
    struct RemoveConst<T const>
    {
        typedef T Result;
    };

    template<typename T>
    struct RemoveQualifiers
    {
        typedef typename RemoveConst<T>::Result Result;
    };

}}

#endif
