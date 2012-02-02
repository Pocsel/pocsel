#ifndef __TOOLS_META_SIGNATURE_HPP__
#define __TOOLS_META_SIGNATURE_HPP__

#include <functional>

#include "TypeList.hpp"

namespace Tools { namespace Meta {

    template<typename T> struct Signature;

    template<typename T> struct Signature<T*> : Signature<T> {};

    template<typename T> struct Signature<T&> : Signature<T> {};

    template<typename T> struct Signature<std::function<T>> : Signature<T> {};


    /////////////////////////////////////////////////////////////////////////////
    // Ret(void)
    template<typename Ret, typename Class>
    struct Signature<Ret(Class::*)(void)> : Signature<Ret(void)> {};

    template<typename Ret> struct Signature<Ret(void)>
    {
        typedef std::function<Ret(void)> Prototype;
        typedef Ret ReturnType;
        typedef TOOLS_META_TYPELIST0() Parameters;
    };

    /////////////////////////////////////////////////////////////////////////////
    // Ret(P1)
    template<typename Ret, typename Class, typename P1>
    struct Signature<Ret(Class::*)(P1)> : Signature<Ret(P1)> {};

    template<typename Ret, typename P1> struct Signature<Ret(P1)>
    {
        typedef std::function<Ret(P1)> Prototype;
        typedef Ret ReturnType;
        typedef TOOLS_META_TYPELIST1(P1) Parameters;
    };

    /////////////////////////////////////////////////////////////////////////////
    // Ret(P1, P2)
    template<typename Ret, typename Class, typename P1, typename P2>
    struct Signature<Ret(Class::*)(P1, P2)> : Signature<Ret(P1, P2)> {};

    template<typename Ret, typename P1, typename P2> struct Signature<Ret(P1, P2)>
    {
        typedef std::function<Ret(P1, P2)> Prototype;
        typedef Ret ReturnType;
        typedef TOOLS_META_TYPELIST2(P1, P2) Parameters;
    };

    /////////////////////////////////////////////////////////////////////////////
    // Ret(P1, P2, P3)
    template<typename Ret, typename Class, typename P1, typename P2, typename P3>
    struct Signature<Ret(Class::*)(P1, P2, P3)> : Signature<Ret(P1, P2, P3)> {};

    template<typename Ret, typename P1, typename P2, typename P3> struct Signature<Ret(P1, P2, P3)>
    {
        typedef std::function<Ret(P1, P2, P3)> Prototype;
        typedef Ret ReturnType;
        typedef TOOLS_META_TYPELIST3(P1, P2, P3) Parameters;
    };

    /////////////////////////////////////////////////////////////////////////////
    // Ret(P1, P2, P3, P4)
    template<typename Ret, typename Class, typename P1, typename P2, typename P3, typename P4>
    struct Signature<Ret(Class::*)(P1, P2, P3, P4)> : Signature<Ret(P1, P2, P3, P4)> {};

    template<typename Ret, typename P1, typename P2, typename P3, typename P4> struct Signature<Ret(P1, P2, P3, P4)>
    {
        typedef std::function<Ret(P1, P2, P3, P4)> Prototype;
        typedef Ret ReturnType;
        typedef TOOLS_META_TYPELIST4(P1, P2, P3, P4) Parameters;
    };

}}

#endif
