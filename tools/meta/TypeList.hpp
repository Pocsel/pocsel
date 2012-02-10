#ifndef __TOOLS_META_TYPELIST_HPP__
#define __TOOLS_META_TYPELIST_HPP__

namespace Tools { namespace Meta {

    struct NullList
    {
        static const size_t size = 0;
    };

    namespace {

        template<typename T>
        struct _GetTypeListSize
        {
            static const size_t result = _GetTypeListSize<typename T::Tail>::result + 1;
        };

        template<>
        struct _GetTypeListSize<NullList>
        {
            static const size_t result = 0;
        };

    }

    template<typename H, typename T> struct TypeList
    {
        typedef H Head;
        typedef T Tail;
        static const size_t size = _GetTypeListSize<T>::result + 1;
    };


/////////////////////////////////////////////////////////////////////////////
// TOOLS_META_TYPELIST(...) macros

#define TOOLS_META_TYPELIST0() \
    Tools::Meta::NullList
#define TOOLS_META_TYPELIST1(T1) \
    Tools::Meta::TypeList<T1, TOOLS_META_TYPELIST0()>
#define TOOLS_META_TYPELIST2(T1, T2) \
    Tools::Meta::TypeList<T1, TOOLS_META_TYPELIST1(T2)>
#define TOOLS_META_TYPELIST3(T1, T2, T3) \
    Tools::Meta::TypeList<T1, TOOLS_META_TYPELIST2(T2, T3)>
#define TOOLS_META_TYPELIST4(T1, T2, T3, T4) \
    Tools::Meta::TypeList<T1, TOOLS_META_TYPELIST3(T2, T3, T4)>
#define TOOLS_META_TYPELIST5(T1, T2, T3, T4, T5) \
    Tools::Meta::TypeList<T1, TOOLS_META_TYPELIST4(T2, T3, T4, T5)>
#define TOOLS_META_TYPELIST6(T1, T2, T3, T4, T5, T6) \
    Tools::Meta::TypeList<T1, TOOLS_META_TYPELIST5(T2, T3, T4, T5, T6)>
#define TOOLS_META_TYPELIST7(T1, T2, T3, T4, T5, T6, T7) \
    Tools::Meta::TypeList<T1, TOOLS_META_TYPELIST6(T2, T3, T4, T5, T6, T7)>
#define TOOLS_META_TYPELIST8(T1, T2, T3, T4, T5, T6, T7, T8) \
    Tools::Meta::TypeList<T1, TOOLS_META_TYPELIST7(T2, T3, T4, T5, T6, T7, T8)>
#define TOOLS_META_TYPELIST9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
    Tools::Meta::TypeList<T1, TOOLS_META_TYPELIST8(T2, T3, T4, T5, T6, T7, T8, T9)>

/////////////////////////////////////////////////////////////////////////////



}}

#endif
