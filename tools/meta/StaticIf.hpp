#ifndef __TOOLS_META_STATICIF_HPP__
#define __TOOLS_META_STATICIF_HPP__

namespace Tools { namespace Meta {

    template<const bool pred, typename Then, typename Else> struct StaticIf
    {
        typedef Then Result;
    };

    template<typename Then, typename Else> struct StaticIf<false, Then, Else>
    {
        typedef Else Result;
    };

}}

#endif
