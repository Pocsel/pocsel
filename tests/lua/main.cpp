#include "tools/precompiled.hpp"

#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Iterator.hpp"
#include "tools/lua/MetaTable.hpp"
#include "tools/lua/utils/Vector.hpp"
#include "tools/Matrix4.hpp"
#include "tools/Timer.hpp"

void f(Tools::Lua::CallHelper& call)
{
    auto const& args = call.GetArgList();
    auto it = args.begin();
    auto itEnd = args.end();
    for (; it != itEnd; ++it)
        Tools::log << "arg: " << it->ToNumber() << std::endl;
    Tools::log << call.PopArg().CheckBoolean() << std::endl;
    Tools::log << call.PopArg().CheckString() << std::endl;
    Tools::log << "f()" << std::endl;
    call.PushRet(call.GetInterpreter().MakeNumber(1.5));
    call.PushRet(call.GetInterpreter().MakeNumber(2.6));
}

using namespace Tools::Lua;

void TestFunction(int test)
{
    std::cout << test << std::endl;
}

void Init(Interpreter& i)
{
    MetaTable m(i, glm::detail::tmat4x4<float>());
    m.SetMetaMethod(MetaTable::Serialize,
        [&i](CallHelper& helper)
        {
            auto& m = *helper.PopArg().Check<glm::detail::tmat4x4<float>*>();
            std::stringstream tmp;
            tmp << m[0][0];
            for (int j = 1; j < 16; ++j)
                tmp << ", " << m[j/4][j%4];
            helper.PushRet(i.MakeString("return Matrix4f.New16(" + tmp.str() + ")"));
        });
//    m.SetMethod("Rotate", [](CallHelper&) { });
    m.SetMethod("Dump",
        [](CallHelper& helper)
        {
            auto& m = *helper.PopArg().Check<glm::detail::tmat4x4<float>*>();
            for (int i = 0; i < 4; ++i)
                Tools::log << m[i][0] << ", " << m[i][1] << ", " << m[i][2] << ", " << m[i][3] << std::endl;
        });
    m.SetMetaMethod(MetaTable::Call, [](CallHelper&) { Tools::log << "call !\n"; });
    m.SetMetaMethod(
        MetaTable::Multiply,// i.Bind(&glm::detail::tmat4x4<float>::operator*, std::placeholders::_1, std::placeholders::_2));
        [m](CallHelper& helper)
        {
            auto& m1 = *helper.PopArg().Check<glm::detail::tmat4x4<float>*>();
            auto& m2 = *helper.PopArg().Check<glm::detail::tmat4x4<float>*>();
            auto m3 = m1 * m2;
            helper.PushRet(helper.GetInterpreter().Make(m3));
        });

    auto n = i.MakeTable();
    n.Set("New", i.MakeFunction(
        [](CallHelper& helper)
        {
            helper.PushRet(helper.GetInterpreter().Make(glm::detail::tmat4x4<float>::identity));
        }));
    n.Set("New16", i.MakeFunction(
        [](CallHelper& helper)
        {
            glm::detail::tmat4x4<float> m;
            for (int i = 0; i < 16; ++i)
                m[i/4][i%4] = (float)helper.PopArg().CheckNumber();
            helper.PushRet(helper.GetInterpreter().Make(m));
        }));
    i.Globals().Set("Matrix4f", n);
}

struct A
{
    A Print(int i)
    {
        std::cout << "AAA" << i << "\n";
        return A();
    }
};

int main(int, char**)
{
    {
        Tools::Lua::Interpreter i;
        i.RegisterLib(Tools::Lua::Interpreter::Base);
        i.RegisterLib(Tools::Lua::Interpreter::Math);

        i.DoString("test = {} test[4] = 23.34");

        Tools::Lua::Ref bite = i.Bind(&f, std::placeholders::_1);
        //Tools::Lua::Ref bite = i.MakeFunction(std::bind(&f, std::placeholders::_1));

        try
        {
            Tools::log << bite(1, 2).ToNumber() << " jksdfgjk " << sizeof(Tools::Lua::Ref) << std::endl;
        }
        catch (std::exception& e)
        {
            Tools::log << "from c++: " << e.what() << std::endl;
        }

        i.Globals().Set(std::string("bite"), bite);

        try
        {
            i.DoString("bite(1, 2)");
        }
        catch (std::exception& e)
        {
            Tools::log << "from lua: " << e.what() << std::endl;
        }

        {
            Tools::Lua::CallHelper helper(i);
            helper.PushArg(i.Make(true));
            helper.PushArg(i.Make("Hey!!"));
            bite(helper);
            auto const& rets = helper.GetRetList();
            auto it = rets.begin();
            auto itEnd = rets.end();
            for (; it != itEnd; ++it)
                Tools::log << "ret: " << it->ToString() << std::endl;
            i.DoString("ret1, ret2 = bite(false, \"ZOB\")");
            Tools::log << "ret1: " << i.Globals()["ret1"].ToString() << ", ret2: " << i.Globals()["ret2"].ToString() << std::endl;
        }

        i.DoString("test = \"sjkldg\\nlkéhsdfg\" g = 323.4 jdsk = {} iwer = {} jkd = function() end");

        Tools::Lua::Iterator it = i.Globals().Begin();
        Tools::Lua::Iterator itEnd = i.Globals().End();
        for (; it != itEnd; ++it)
        {
            Tools::log << it.GetValue().GetTypeName() << ": " << it.GetValue().ToString() << " (key " << it.GetKey().GetTypeName() << " " << it.GetKey().ToString() << ")" << std::endl;
        }

        i.DoString("FUCK = function() print \"FUCK\" end");
        Tools::Lua::Ref meta = i.MakeTable();
        meta.Set("__index", i.Globals()["FUCK"]);
        meta.Set(1, "test");
        meta.Set(2, "tests2");
        Tools::log << "Type of FUCK: " << meta["__index"].GetTypeName() << std::endl;
        Tools::log << "size of metatable: " << meta.GetLength() << std::endl;
        Tools::Lua::Ref metaTest = i.MakeTable();
        metaTest.SetMetaTable(meta);
        i.Globals().Set("metaTest", metaTest);
        i.DoString("a = metaTest[12]");
        i.DoString("b = metaTest[14]");
        i.DoString("c = metaTest[15]");

        i.DoString("sharedTable = { 213, 23, \"sdfg\" }");
        i.DoString("serializeTest = { bite = { 1, 3, 4, sharedTable }, test = \"HEY\", allo = false }");
        std::string serialized = i.GetSerializer().Serialize(i.Globals()["serializeTest"]);
        Tools::log << "serialize1: \"\"\"" << serialized << "\"\"\"\n";
        try
        {
            Tools::log << "serialize2: \"\"\"" << i.GetSerializer().Serialize(i.Globals()["FUCK"]) << "\"\"\"\n";
        }
        catch (std::exception& e)
        {
            Tools::log << "serialize fail: " << e.what() << std::endl;
        }
        Tools::log << "serialize3: \"\"\"" << i.GetSerializer().Serialize(i.Globals()["test"]) << "\"\"\"\n";
        Tools::log << "Deserialize: \"\"\"" << i.GetSerializer().Serialize(i.GetSerializer().Deserialize(serialized)) << "\"\"\"\n";

        Init(i);
        i.DoString("m = Matrix4f.New()");
        i.DoString("m:Dump()");
        i.DoString("m()");
        i.DoString("m2 = Matrix4f.New()");
        i.DoString("m3 = m * m2");
        i.DoString("m3:Dump()");

        MetaTable mA(i, A());
        i.Globals().Set("NewA", i.MakeFunction([&i](CallHelper& helper) { helper.PushRet(i.Make(A())); }));

        try
        {
            i.DoString("m.Dump(NewA())");
        }
        catch (std::exception& e)
        {
            Tools::log << "call \"Matrix4f::Dump\" with a variable of type \"A\": " << e.what() << "\n";
        }
        try
        {
            i.DoString("table = getmetatable(m)");
        }
        catch (std::exception& e)
        {
            Tools::log << "getmetatable(UserData): " << e.what() << "\n";
        }

        i.Globals().Set("TestFunction", i.Bind(&TestFunction, std::placeholders::_1));
        i.DoString("TestFunction(10)");
        auto r = i.Globals().Set("A", i.Bind(&A::Print, std::placeholders::_1, std::placeholders::_2));
        r(A(), 50);

        //// glm::dvec2 - MetaTable
        MetaTable mVec2(i, glm::dvec2());
        mVec2.SetMethod("Normalize", i.Bind(static_cast<glm::dvec2(*)(glm::dvec2 const&)>(&glm::normalize), std::placeholders::_1));
        mVec2.SetMethod("Dot", i.Bind(static_cast<double(*)(glm::dvec2 const&, glm::dvec2 const&)>(&glm::dot), std::placeholders::_1, std::placeholders::_2));
        mVec2.SetMetaMethod(MetaTable::Serialize,
            [&i](CallHelper& helper)
            {
                auto v = helper.PopArg().Check<glm::dvec2*>();
                helper.PushRet(i.MakeString("return Vec2.New(" + Tools::ToString(v->x) + ", " + Tools::ToString(v->y) + ")"));
            });
        mVec2.SetMethod("Dump",
            [](CallHelper& helper)
            {
                auto v = helper.PopArg().Check<glm::dvec2*>();
                Tools::log << "(" << v->x << ";" << v->y << ")\n";
            });
        // Namespace
        auto n = i.MakeTable();
        n.Set("New", i.MakeFunction(
            [&i](CallHelper& helper)
            {
                auto x = helper.PopArg().Check<double>();
                auto y = helper.PopArg().Check<double>();
                helper.PushRet(i.Make(glm::dvec2(x, y)));
            }));
        i.Globals().Set("Vec2", n);
        i.DoString(
            "v = Vec2.New(2, 3)"
            "v:Dump()"
            "v:Normalize()"
            "v:Dump()");

        i.DoString("tab = { [v] = m, [2] = m1, ['toto'] = m2 }");
        Tools::log << "first: " << i.GetSerializer().Serialize(i.Globals()["tab"]) << std::endl;
        Tools::log << "second: " << i.GetSerializer().Serialize(i.GetSerializer().Deserialize(i.GetSerializer().Serialize(i.Globals()["tab"]))) << std::endl;

        i.DumpStack();


        Tools::log << "------- Begin super test de serialization -------" << std::endl;
        auto testObj = i.MakeTable();
        auto testObjMetaTable = i.MakeTable();
        i.DoString("testObjSerialize = function(obj) return \"return nil\" end -- lol");
        testObjMetaTable.Set("__serialize", i.Globals()["testObjSerialize"]);
        testObj.SetMetaTable(testObjMetaTable);
        auto t = i.MakeTable();
        t.Set("h", i.MakeFunction(std::bind(&f, std::placeholders::_1)));
        t.Set("obj", testObj);
        try
        {
            Tools::log << i.GetSerializer().Serialize(t, true) << std::endl;
        }
        catch (std::exception& e)
        {
            Tools::log << "error: " << e.what() << std::endl;
        }

        MetaTable vec3(i, glm::vec3());
        vec3.SetMetaMethod(MetaTable::Add,
            [&](CallHelper& helper)
            {
                auto rhs = *helper.PopArg().To<glm::vec3*>();
                auto lhs = helper.PopArg();
                if (lhs.IsNumber())
                    helper.PushRet(i.Make(rhs + (float)lhs.ToNumber()));
                else if (lhs.Is<glm::vec3>())
                    helper.PushRet(i.Make(rhs + *lhs.To<glm::vec3*>()));
            });
        i.Globals().Set("vec3", i.MakeFunction(
            [&](CallHelper& helper)
            {
                auto arg = helper.PopArg();
                if (arg.IsTable())
                    helper.PushRet(i.Make<glm::vec3>(Utils::Vector::TableToVec3<float>(arg)));
                else
                {
                    glm::vec3 v((float)arg.ToNumber(), 0.0f, 0.0f);
                    v.y = (float)helper.PopArg().ToNumber();
                    v.z = (float)helper.PopArg().ToNumber();
                    helper.PushRet(i.Make(v));
                }
            }));

#define TOSTR(...) #__VA_ARGS__

        i.DoString(TOSTR(
            vec3mt = {
                __add = function (a, b)
                    c = { x = a.x + b.x, y = a.y + b.y, z = a.z + b.z }
                    setmetatable(c, vec3mt)
                    return c
                end
            }
            function vec3(x, y, z)
                local v = {}
                if type(x) == "table" then
                    v = { x = x.x, y = x.y, z = x.z }
                else
                    v = { x = x, y = y, z = z }
                end
                setmetatable(v, vec3mt)
                return v
            end

            mat4mt = {
                __mul = function (m1, m2)
                    local m1m00 = m1.m00
                    local m1m01 = m1.m01
                    local m1m02 = m1.m02
                    local m1m03 = m1.m03
                    local m1m10 = m1.m10
                    local m1m11 = m1.m11
                    local m1m12 = m1.m12
                    local m1m13 = m1.m13
                    local m1m20 = m1.m20
                    local m1m21 = m1.m21
                    local m1m22 = m1.m22
                    local m1m23 = m1.m23
                    local m1m30 = m1.m30
                    local m1m31 = m1.m31
                    local m1m32 = m1.m32
                    local m1m33 = m1.m33
        
                    local m2m00 = m2.m00
                    local m2m01 = m2.m01
                    local m2m02 = m2.m02
                    local m2m03 = m2.m03
                    local m2m10 = m2.m10
                    local m2m11 = m2.m11
                    local m2m12 = m2.m12
                    local m2m13 = m2.m13
                    local m2m20 = m2.m20
                    local m2m21 = m2.m21
                    local m2m22 = m2.m22
                    local m2m23 = m2.m23
                    local m2m30 = m2.m30
                    local m2m31 = m2.m31
                    local m2m32 = m2.m32
                    local m2m33 = m2.m33
        
                    r = {
                        m00 = m1m00 * m2m00 + m1m01 * m2m10 + m1m02 * m2m20 + m1m03 * m2m30,
                        m01 = m1m00 * m2m01 + m1m01 * m2m11 + m1m02 * m2m21 + m1m03 * m2m31,
                        m02 = m1m00 * m2m02 + m1m01 * m2m12 + m1m02 * m2m22 + m1m03 * m2m32,
                        m03 = m1m00 * m2m03 + m1m01 * m2m13 + m1m02 * m2m23 + m1m03 * m2m33,
                        m10 = m1m10 * m2m00 + m1m11 * m2m10 + m1m12 * m2m20 + m1m13 * m2m30,
                        m11 = m1m10 * m2m01 + m1m11 * m2m11 + m1m12 * m2m21 + m1m13 * m2m31,
                        m12 = m1m10 * m2m02 + m1m11 * m2m12 + m1m12 * m2m22 + m1m13 * m2m32,
                        m13 = m1m10 * m2m03 + m1m11 * m2m13 + m1m12 * m2m23 + m1m13 * m2m33,
                        m20 = m1m20 * m2m00 + m1m21 * m2m10 + m1m22 * m2m20 + m1m23 * m2m30,
                        m21 = m1m20 * m2m01 + m1m21 * m2m11 + m1m22 * m2m21 + m1m23 * m2m31,
                        m22 = m1m20 * m2m02 + m1m21 * m2m12 + m1m22 * m2m22 + m1m23 * m2m32,
                        m23 = m1m20 * m2m03 + m1m21 * m2m13 + m1m22 * m2m23 + m1m23 * m2m33,
                        m30 = m1m30 * m2m00 + m1m31 * m2m10 + m1m32 * m2m20 + m1m33 * m2m30,
                        m31 = m1m30 * m2m01 + m1m31 * m2m11 + m1m32 * m2m21 + m1m33 * m2m31,
                        m32 = m1m30 * m2m02 + m1m31 * m2m12 + m1m32 * m2m22 + m1m33 * m2m32,
                        m33 = m1m30 * m2m03 + m1m31 * m2m13 + m1m32 * m2m23 + m1m33 * m2m33,
                    }
                    setmetatable(r, mat4mt)
                    return r
                end
            }
            function Matrix4f.New()
                r = {
                    m00 = 1, m01 = 0, m02 = 0, m03 = 0,
                    m10 = 0, m11 = 1, m12 = 0, m13 = 0,
                    m20 = 0, m21 = 0, m22 = 1, m23 = 0,
                    m30 = 0, m31 = 0, m32 = 0, m33 = 1,
                }
                setmetatable(r, mat4mt)
                return r
            end
        ));

        Tools::Timer timer;
        Tools::log << "Benchmark vector 3\n";
        i.DoString("a = vec3(1.5, 2.5, 3.5)");
        i.DoString("b = vec3(-1.5, -2.5, -3.5)");
        i.DoString("for i=1,5000 do c = a + b end");
        Tools::log << "End (time: " << timer.GetPreciseElapsedTime() << " us)\n";

        timer.Reset();
        Tools::log << "Benchmark matrix 4\n";
        i.DoString("m = Matrix4f.New()");
        i.DoString("m2 = Matrix4f.New()");
        i.DoString("for i=1,5000 do m3 = m * m2 end");
        //for (int _ = 0; _ < 5000; ++_)
        //{
        //    //i.Globals()["m"].Check<glm::mat4*>();
        //    auto matrix = i.Globals()["m"];
        //    if (matrix.IsTable())
        //    {
        //        glm::mat4 m(
        //            matrix["m00"].Check<float>(), matrix["m01"].Check<float>(), matrix["m02"].Check<float>(), matrix["m03"].Check<float>(),
        //            matrix["m10"].Check<float>(), matrix["m11"].Check<float>(), matrix["m12"].Check<float>(), matrix["m13"].Check<float>(),
        //            matrix["m20"].Check<float>(), matrix["m21"].Check<float>(), matrix["m22"].Check<float>(), matrix["m23"].Check<float>(),
        //            matrix["m30"].Check<float>(), matrix["m31"].Check<float>(), matrix["m32"].Check<float>(), matrix["m33"].Check<float>());
        //    }
        //}
        Tools::log << "End (time: " << timer.GetPreciseElapsedTime() << " us)\n";

        Tools::log << "Benchmark matrix 4 C++\n";
        timer.Reset();
        glm::mat4 m;
        glm::mat4 m2;
        glm::mat4 m3;
        for (int _ = 0; _ < 100000; ++_)
        {
            m3 += m * m2;
        }
        Tools::log << "End (time: " << timer.GetPreciseElapsedTime() << " us)\n";
        for (int i = 0; i < 3; ++i)
            Tools::log << m3[i].x << " " << m3[i].y << " " << m3[i].x << " " << m3[i].z << std::endl;
    }


#ifdef _WIN32
    std::cin.get();
#endif
    return 0;
}
