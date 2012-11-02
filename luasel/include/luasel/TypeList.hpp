#ifndef  __LUASEL_TYPELIST_HPP__
# define __LUASEL_TYPELIST_HPP__

namespace Luasel {

    struct NullList
    {
        static const size_t size = 0;
    };

    template<class TList>
        struct _GetSize
        {
            static const size_t value = _GetSize<typename TList::Tail>::value + 1;
        };
    template<>
        struct _GetSize<NullList>
        {
            static const size_t value = 0;
        };

    template<class THead, class TTail>
    struct TypeList
    {
    private:

    public:
        typedef THead Head;
        typedef TTail Tail;

        static const size_t size = _GetSize<Tail>::value + 1;
    };


/////////////////////////////////////////////////////////////////////////////
// LUASEL_TYPELIST(...) macros

# define LUASEL_TYPELIST0() \
    Luasel::NullList
# define LUASEL_TYPELIST1(T1) \
    Luasel::TypeList<T1, LUASEL_TYPELIST0()>
# define LUASEL_TYPELIST2(T1, T2) \
    Luasel::TypeList<T1, LUASEL_TYPELIST1(T2)>
# define LUASEL_TYPELIST3(T1, T2, T3) \
    Luasel::TypeList<T1, LUASEL_TYPELIST2(T2, T3)>
# define LUASEL_TYPELIST4(T1, T2, T3, T4) \
    Luasel::TypeList<T1, LUASEL_TYPELIST3(T2, T3, T4)>
# define LUASEL_TYPELIST5(T1, T2, T3, T4, T5) \
    Luasel::TypeList<T1, LUASEL_TYPELIST4(T2, T3, T4, T5)>
# define LUASEL_TYPELIST6(T1, T2, T3, T4, T5, T6) \
    Luasel::TypeList<T1, LUASEL_TYPELIST5(T2, T3, T4, T5, T6)>
# define LUASEL_TYPELIST7(T1, T2, T3, T4, T5, T6, T7) \
    Luasel::TypeList<T1, LUASEL_TYPELIST6(T2, T3, T4, T5, T6, T7)>
# define LUASEL_TYPELIST8(T1, T2, T3, T4, T5, T6, T7, T8) \
    Luasel::TypeList<T1, LUASEL_TYPELIST7(T2, T3, T4, T5, T6, T7, T8)>
# define LUASEL_TYPELIST9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
    Luasel::TypeList<T1, LUASEL_TYPELIST8(T2, T3, T4, T5, T6, T7, T8, T9)>

}

#endif
