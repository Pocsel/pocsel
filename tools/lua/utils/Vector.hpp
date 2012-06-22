#ifndef __TOOLS_LUA_VECTOR_HPP__
#define __TOOLS_LUA_VECTOR_HPP__

#include "tools/Vector3.hpp"
#include "tools/Vector2.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Iterator.hpp"

namespace Tools { namespace Lua { namespace Utils { namespace Vector {

    // Vector 2
    template<typename T>
    glm::detail::tvec2<T> RefToVector2(Ref const& ref) throw(std::runtime_error)
    {
        if (!ref.IsTable())
            throw std::runtime_error("TableToVector2: Value is of " + ref.GetTypeName() + " type (expected table)");
        return glm::detail::tvec2<T>(
                ref["x"].Check<T>("TableToVector2: Invalid type for field \"x\""),
                ref["y"].Check<T>("TableToVector2: Invalid type for field \"y\""));
    }

    template<typename T>
    void Vector2ToRef(glm::detail::tvec2<T> const& v, Ref& ref) throw(std::runtime_error)
    {
        ref = ref.GetState().MakeTable();
        ref.Set("x", v.x);
        ref.Set("y", v.y);
    }

    // Vector 3
    template<typename T>
    glm::detail::tvec3<T> RefToVector3(Ref const& ref) throw(std::runtime_error)
    {
        if (!ref.IsTable())
            throw std::runtime_error("TableToVector3: Value is of " + ref.GetTypeName() + " type (expected table)");
        return glm::detail::tvec3<T>(
                ref["x"].Check<T>("TableToVector3: Invalid type for field \"x\""),
                ref["y"].Check<T>("TableToVector3: Invalid type for field \"y\""),
                ref["z"].Check<T>("TableToVector3: Invalid type for field \"z\""));
    }

    template<typename T>
    void Vector3ToRef(glm::detail::tvec3<T> const& v, Ref& ref) throw(std::runtime_error)
    {
        ref = ref.GetState().MakeTable();
        ref.Set("x", v.x);
        ref.Set("y", v.y);
        ref.Set("z", v.z);
    }

    // Vector 4
    template<typename T>
    glm::detail::tvec4<T> RefToVector4(Ref const& ref) throw(std::runtime_error)
    {
        if (!ref.IsTable())
            throw std::runtime_error("TableToVector3: Value is of " + ref.GetTypeName() + " type (expected table)");
        return glm::detail::tvec4<T>(
                ref["x"].Check<T>("TableToVector3: Invalid type for field \"x\""),
                ref["y"].Check<T>("TableToVector3: Invalid type for field \"y\""),
                ref["z"].Check<T>("TableToVector3: Invalid type for field \"z\""),
                ref["w"].Check<T>("TableToVector3: Invalid type for field \"w\""));
    }

    template<typename T>
    void Vector4ToRef(glm::detail::tvec3<T> const& v, Ref& ref) throw(std::runtime_error)
    {
        ref = ref.GetState().MakeTable();
        ref.Set("x", v.x);
        ref.Set("y", v.y);
        ref.Set("z", v.z);
        ref.Set("w", v.w);
    }

}}}}

#endif
