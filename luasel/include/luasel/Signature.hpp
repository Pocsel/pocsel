#ifndef __LUASEL_SIGNATURE_HPP__
#define __LUASEL_SIGNATURE_HPP__

#include <functional>
#include <luasel/TypeList.hpp>

namespace Luasel {

    template<typename T>
    struct Signature;

    template<typename T>
    struct Signature<T*> : public Signature<T>
    {
    };

    template<typename T>
    struct Signature<T&> : public Signature<T>
    {
    };

    template<typename T>
    struct Signature<std::function<T>> : public Signature<T>
    {
    };

    /////////////////////////////////////////////////////////////////////////////
    // Ret(void)
    template<typename Ret, typename Class>
    struct Signature<Ret(Class::*)(void)> : public Signature<Ret(Class*)> {};

    template<typename Ret>
    struct Signature<Ret(void)>
    {
        typedef std::function<Ret(void)> Prototype;
        typedef Ret ReturnType;
        typedef LUASEL_TYPELIST0() Parameters;
    };

    /////////////////////////////////////////////////////////////////////////////
    // Ret(P1)
    template<typename Ret, typename Class, typename P1>
    struct Signature<Ret(Class::*)(P1)> : public Signature<Ret(Class*, P1)> {};

    template<typename Ret, typename P1>
    struct Signature<Ret(P1)>
    {
        typedef std::function<Ret(P1)> Prototype;
        typedef Ret ReturnType;
        typedef LUASEL_TYPELIST1(P1) Parameters;
    };

    /////////////////////////////////////////////////////////////////////////////
    // Ret(P1, P2)
    template<typename Ret, typename Class, typename P1, typename P2>
    struct Signature<Ret(Class::*)(P1, P2)> : public Signature<Ret(Class*, P1, P2)> {};

    template<typename Ret, typename P1, typename P2>
    struct Signature<Ret(P1, P2)>
    {
        typedef std::function<Ret(P1, P2)> Prototype;
        typedef Ret ReturnType;
        typedef LUASEL_TYPELIST2(P1, P2) Parameters;
    };

    /////////////////////////////////////////////////////////////////////////////
    // Ret(P1, P2, P3)
    template<typename Ret, typename Class, typename P1, typename P2, typename P3>
    struct Signature<Ret(Class::*)(P1, P2, P3)> : public Signature<Ret(Class*, P1, P2, P3)> {};

    template<typename Ret, typename P1, typename P2, typename P3>
    struct Signature<Ret(P1, P2, P3)>
    {
        typedef std::function<Ret(P1, P2, P3)> Prototype;
        typedef Ret ReturnType;
        typedef LUASEL_TYPELIST3(P1, P2, P3) Parameters;
    };

    /////////////////////////////////////////////////////////////////////////////
    // Ret(P1, P2, P3, P4)
    //template<typename Ret, typename Class, typename P1, typename P2, typename P3, typename P4>
    //struct Signature<Ret(Class::*)(P1, P2, P3, P4)> : public Signature<Ret(P1, P2, P3, P4)> {};

    template<typename Ret, typename P1, typename P2, typename P3, typename P4>
    struct Signature<Ret(P1, P2, P3, P4)>
    {
        typedef std::function<Ret(P1, P2, P3, P4)> Prototype;
        typedef Ret ReturnType;
        typedef LUASEL_TYPELIST4(P1, P2, P3, P4) Parameters;
    };

}

#endif
