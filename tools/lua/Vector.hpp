#ifndef __TOOLS_LUA_VECTOR_HPP__
#define __TOOLS_LUA_VECTOR_HPP__

#include "tools/Vector3.hpp"
#include "tools/Vector2.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Iterator.hpp"

namespace Tools { namespace Lua { namespace Vector {

    template<typename T>
        Vector3<T> TableToVector3(Ref const& ref) throw(std::runtime_error)
        {
            if (!ref.IsTable())
                throw std::runtime_error("TableToVector3: Value is of " + ref.GetTypeName() + " type (expected table)");
            if (ref.GetLength() != 3)
                throw std::runtime_error("TableToVector3: Table has a length of " + ToString(ref.GetLength()) + " (expected 3)");
            return Vector3<T>(
                    ref["x"].Check<T>("TableToVector2: Invalid type for field \"x\""),
                    ref["y"].Check<T>("TableToVector2: Invalid type for field \"y\""),
                    ref["z"].Check<T>("TableToVector2: Invalid type for field \"z\""));
        }

    template<typename T>
        void Vector3ToTable(Vector3<T> v, Ref& ref) throw(std::runtime_error)
        {
            ref = ref.GetState().MakeTable();
            ref.Set("x", v.x);
            ref.Set("y", v.y);
            ref.Set("z", v.z);
        }

    template<typename T>
        Vector2<T> TableToVector2(Ref const& ref) throw(std::runtime_error)
        {
            if (!ref.IsTable())
                throw std::runtime_error("TableToVector2: Value is of " + ref.GetTypeName() + " type (expected table)");
            if (ref.GetLength() != 2)
                throw std::runtime_error("TableToVector2: Table has a length of " + ToString(ref.GetLength()) + " (expected 2)");
            return Vector2<T>(
                    ref["x"].Check<T>("TableToVector2: Invalid type for field \"x\""),
                    ref["y"].Check<T>("TableToVector2: Invalid type for field \"y\""));
        }

    template<typename T>
        void Vector2ToTable(Vector2<T> v, Ref& ref) throw(std::runtime_error)
        {
            ref = ref.GetState().MakeTable();
            ref.Set("x", v.x);
            ref.Set("y", v.y);
        }

}}}

#endif
