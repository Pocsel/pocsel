#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/utils/Utils.hpp"

namespace Tools { namespace Lua { namespace Utils {

#define data luaMatrixCode
#include "tools/lua/utils/Matrix.lua.hpp"
#undef data

    namespace {
        template<class T>
        Ref MakeRef(Ref metaTable, glm::detail::tmat4x4<T> const* value)
        {
            auto const& m = *value;
            auto table = metaTable.GetState().MakeTable();
            table.Set("m00", m[0][0]);
            table.Set("m10", m[1][0]);
            table.Set("m20", m[2][0]);
            table.Set("m30", m[3][0]);
            table.Set("m01", m[0][1]);
            table.Set("m11", m[1][1]);
            table.Set("m21", m[2][1]);
            table.Set("m31", m[3][1]);
            table.Set("m02", m[0][2]);
            table.Set("m12", m[1][2]);
            table.Set("m22", m[2][2]);
            table.Set("m32", m[3][2]);
            table.Set("m03", m[0][3]);
            table.Set("m13", m[1][3]);
            table.Set("m23", m[2][3]);
            table.Set("m33", m[3][3]);
            table.SetMetaTable(metaTable);
            return table;
        }
        template<class T>
        void MakeNative(Ref const& ref, glm::detail::tmat4x4<T>* value)
        {
            auto& m = *value;
            m[0][0] = T(ref["m00"].ToNumber());
            m[1][0] = T(ref["m10"].ToNumber());
            m[2][0] = T(ref["m20"].ToNumber());
            m[3][0] = T(ref["m30"].ToNumber());
            m[0][1] = T(ref["m01"].ToNumber());
            m[1][1] = T(ref["m11"].ToNumber());
            m[2][1] = T(ref["m21"].ToNumber());
            m[3][1] = T(ref["m31"].ToNumber());
            m[0][2] = T(ref["m02"].ToNumber());
            m[1][2] = T(ref["m12"].ToNumber());
            m[2][2] = T(ref["m22"].ToNumber());
            m[3][2] = T(ref["m32"].ToNumber());
            m[0][3] = T(ref["m03"].ToNumber());
            m[1][3] = T(ref["m13"].ToNumber());
            m[2][3] = T(ref["m23"].ToNumber());
            m[3][3] = T(ref["m33"].ToNumber());
        }
    }

    void RegisterMatrix(Lua::Interpreter& interpreter)
    {
        RegisterVector(interpreter);
        if (!interpreter.Globals().GetTable("Utils")["Matrix"].Exists())
        {
            auto module = interpreter.LoadString(std::string((char const*)luaMatrixCode, sizeof(luaMatrixCode)))();

            auto mat4mt = module["matrix4MetaTable"];

            MetaTable::Create(mat4mt, glm::mat4(), [=](void const* ptr) { return MakeRef(mat4mt, (glm::mat4 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::mat4*)value); });
            MetaTable::Create(mat4mt, glm::dmat4(), [=](void const* ptr) { return MakeRef(mat4mt, (glm::dmat4 const*)ptr); }, [=](Ref const& ref, void* value) { MakeNative(ref, (glm::dmat4*)value); });
        }
    }

}}}
