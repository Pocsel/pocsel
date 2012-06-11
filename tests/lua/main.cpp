#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Iterator.hpp"
#include "tools/lua/MetaTable.hpp"
#include "tools/Matrix4.hpp"

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
    }
#ifdef _WIN32
    std::cin.get();
#endif
    return 0;
}
