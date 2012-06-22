#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/utils/Utils.hpp"

namespace Tools { namespace Lua { namespace Utils {

#define data luaVectorCode
#include "tools/lua/utils/Vector.lua.hpp"
#undef data

    namespace {
        template<class T>
        Ref MakeRef(Ref metaTable, glm::detail::tvec2<T> const* value)
        {
            auto table = metaTable.GetState().MakeTable();
            table.Set("x", value->x);
            table.Set("y", value->y);
            table.SetMetaTable(metaTable);
            return table;
        }
        template<class T>
        void MakeNative(Ref const& ref, glm::detail::tvec2<T>* value)
        {
            value->x = T(ref["x"].CheckNumber());
            value->y = T(ref["y"].CheckNumber());
        }

        template<class T>
        Ref MakeRef(Ref metaTable, glm::detail::tvec3<T> const* value)
        {
            auto table = metaTable.GetState().MakeTable();
            table.Set("x", value->x);
            table.Set("y", value->y);
            table.Set("z", value->z);
            table.SetMetaTable(metaTable);
            return table;
        }
        template<class T>
        void MakeNative(Ref const& ref, glm::detail::tvec3<T>* value)
        {
            value->x = T(ref["x"].CheckNumber());
            value->y = T(ref["y"].CheckNumber());
            value->z = T(ref["z"].CheckNumber());
        }

        template<class T>
        Ref MakeRef(Ref metaTable, glm::detail::tvec4<T> const* value)
        {
            auto table = metaTable.GetState().MakeTable();
            table.Set("x", value->x);
            table.Set("y", value->y);
            table.Set("z", value->z);
            table.Set("w", value->w);
            table.SetMetaTable(metaTable);
            return table;
        }
        template<class T>
        void MakeNative(Ref const& ref, glm::detail::tvec4<T>* value)
        {
            value->x = T(ref["x"].CheckNumber());
            value->y = T(ref["y"].CheckNumber());
            value->z = T(ref["z"].CheckNumber());
            value->w = T(ref["w"].CheckNumber());
        }
    }

    void RegisterVector(Lua::Interpreter& interpreter)
    {
        if (!interpreter.Globals().GetTable("Utils")["Vector2"].Exists())
        {
            auto module = interpreter.LoadString(std::string((char const*)luaVectorCode, sizeof(luaVectorCode)))();

            auto vec2mt = module["vector2MetaTable"];
            auto vec3mt = module["vector3MetaTable"];
            auto vec4mt = module["vector4MetaTable"];

            // Int
            MetaTable::Create(vec2mt, glm::ivec2(), [=](void const* ptr) { return MakeRef(vec2mt, (glm::ivec2 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::ivec2*)value); });
            MetaTable::Create(vec3mt, glm::ivec3(), [=](void const* ptr) { return MakeRef(vec3mt, (glm::ivec3 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::ivec3*)value); });
            MetaTable::Create(vec4mt, glm::ivec4(), [=](void const* ptr) { return MakeRef(vec4mt, (glm::ivec4 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::ivec4*)value); });

            // UInt
            MetaTable::Create(vec2mt, glm::uvec2(), [=](void const* ptr) { return MakeRef(vec2mt, (glm::uvec2 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::uvec2*)value); });
            MetaTable::Create(vec3mt, glm::uvec3(), [=](void const* ptr) { return MakeRef(vec3mt, (glm::uvec3 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::uvec3*)value); });
            MetaTable::Create(vec4mt, glm::uvec4(), [=](void const* ptr) { return MakeRef(vec4mt, (glm::uvec4 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::uvec4*)value); });

            // Float
            MetaTable::Create(vec2mt, glm::vec2(), [=](void const* ptr) { return MakeRef(vec2mt, (glm::vec2 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::vec2*)value); });
            MetaTable::Create(vec3mt, glm::vec3(), [=](void const* ptr) { return MakeRef(vec3mt, (glm::vec3 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::vec3*)value); });
            MetaTable::Create(vec4mt, glm::vec4(), [=](void const* ptr) { return MakeRef(vec4mt, (glm::vec4 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::vec4*)value); });

            // Double
            MetaTable::Create(vec2mt, glm::dvec2(), [=](void const* ptr) { return MakeRef(vec2mt, (glm::dvec2 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::dvec2*)value); });
            MetaTable::Create(vec3mt, glm::dvec3(), [=](void const* ptr) { return MakeRef(vec3mt, (glm::dvec3 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::dvec3*)value); });
            MetaTable::Create(vec4mt, glm::dvec4(), [=](void const* ptr) { return MakeRef(vec4mt, (glm::dvec4 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::dvec4*)value); });
        }
    }

}}}
