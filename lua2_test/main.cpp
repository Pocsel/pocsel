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
        Tools::log << "arg: " << it->ToNumber() << Tools::endl;
    Tools::log << call.PopArg().CheckBoolean() << Tools::endl;
    Tools::log << call.PopArg().CheckString() << Tools::endl;
    Tools::log << "f()" << Tools::endl;
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
    auto n = i.MakeTable();
    MetaTable m(i, Tools::Matrix4<float>());
    i.Globals().Set("Matrix4f", n);
    m.AddMethod("Rotate", [](CallHelper& helper) { });
    m.AddMethod("Dump",
        [](CallHelper& helper)
        {
            auto m = helper.PopArg().Check<Tools::Matrix4<float>*>();
            for (int i = 0; i < 4; ++i)
                Tools::log << m->m[i][0] << ", " << m->m[i][1] << ", " << m->m[i][2] << ", " << m->m[i][3] << "\n";
        });
    m.AddMetaMethod(MetaTable::Call,
        [](CallHelper& helper) { Tools::log << "call !\n"; });
    m.AddMetaMethod(
        MetaTable::Collect,
        [](CallHelper& helper) { helper.PopArg().Check<Tools::Matrix4<float>*>()->~Matrix4<float>(); });
    //i.Bind(&Tools::Matrix4<float>::Translate, std::placeholders::_1, std::placeholders::_2);
    //i.Bind(&Tools::Vector2f::Dot, std::placeholders::_1, std::placeholders::_2);
    m.AddMetaMethod(
        MetaTable::Multiply,// i.Bind(&Tools::Matrix4<float>::operator*, std::placeholders::_1, std::placeholders::_2));
        [m](CallHelper& helper)
        {
            auto m1 = helper.PopArg().Check<Tools::Matrix4<float>*>();
            auto m2 = helper.PopArg().Check<Tools::Matrix4<float>*>();

            helper.PushRet(helper.GetInterpreter().Make(m1->operator *(*m2)));
        });

    n.Set("New", i.MakeFunction(
        [](CallHelper& helper)
        {
            helper.PushRet(helper.GetInterpreter().Make(Tools::Matrix4<float>::identity));
        }));
}

class A
{
public:
    A Print(int i)
    {
        std::cout << "AAA" << i << "\n";
        return A();
    }
};

int main(int, char**)
{
    Tools::Lua::Interpreter i;
    i.RegisterLib(Tools::Lua::Interpreter::Base);
    i.RegisterLib(Tools::Lua::Interpreter::Math);

    i.DoString("test = {} test[4] = 23.34");

    Tools::Lua::Ref bite = i.Bind(&f, std::placeholders::_1);
    //Tools::Lua::Ref bite = i.MakeFunction(std::bind(&f, std::placeholders::_1));

    try
    {
        Tools::log << bite(1, 2).ToNumber() << " jksdfgjk " << sizeof(Tools::Lua::Ref) << Tools::endl;
    }
    catch (std::exception& e)
    {
        Tools::log << "from c++: " << e.what() << Tools::endl;
    }

    i.Globals().Set(std::string("bite"), bite);

    try
    {
        i.DoString("bite(1, 2)");
    }
    catch (std::exception& e)
    {
        Tools::log << "from lua: " << e.what() << Tools::endl;
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
            Tools::log << "ret: " << it->ToString() << Tools::endl;
        i.DoString("ret1, ret2 = bite(false, \"ZOB\")");
        Tools::log << "ret1: " << i.Globals()["ret1"].ToString() << ", ret2: " << i.Globals()["ret2"].ToString() << Tools::endl;
    }

    i.DoString("test = \"sjkldg\\nlkéhsdfg\" g = 323.4 jdsk = {} iwer = {} jkd = function() end");

    Tools::Lua::Iterator it = i.Globals().Begin();
    Tools::Lua::Iterator itEnd = i.Globals().End();
    for (; it != itEnd; ++it)
    {
        Tools::log << it.GetValue().GetTypeName() << ": " << it.GetValue().ToString() << " (key " << it.GetKey().GetTypeName() << " " << it.GetKey().ToString() << ")" << Tools::endl;
    }

    i.DoString("FUCK = function() print \"FUCK\" end");
    Tools::Lua::Ref meta = i.MakeTable();
    meta.Set("__index", i.Globals()["FUCK"]);
    meta.Set(1, "test");
    meta.Set(2, "tests2");
    Tools::log << "Type of FUCK: " << meta["__index"].GetTypeName() << Tools::endl;
    Tools::log << "size of metatable: " << meta.GetLength() << Tools::endl;
    Tools::Lua::Ref metaTest = i.MakeTable();
    metaTest.SetMetaTable(meta);
    i.Globals().Set("metaTest", metaTest);
    i.DoString("a = metaTest[12]");
    i.DoString("b = metaTest[14]");
    i.DoString("c = metaTest[15]");

    i.DoString("sharedTable = { 213, 23, \"sdfg\" }");
    i.DoString("serializeTest = { bite = { 1, 3, 4, sharedTable }, test = \"HEY\", allo = false }");
    std::string serialized = i.Serialize(i.Globals()["serializeTest"]);
    Tools::log << "serialize1: \"\"\"" << serialized << "\"\"\"\n";
    try
    {
        Tools::log << "serialize2: \"\"\"" << i.Serialize(i.Globals()["FUCK"]) << "\"\"\"\n";
    }
    catch (std::exception& e)
    {
        Tools::log << "serialize fail: " << e.what() << Tools::endl;
    }
    Tools::log << "serialize3: \"\"\"" << i.Serialize(i.Globals()["test"]) << "\"\"\"\n";
    Tools::log << "Deserialize: \"\"\"" << i.Serialize(i.Deserialize(serialized)) << "\"\"\"\n";

    Init(i);
    i.DoString("m = Matrix4f.New()");
    i.DoString("m:Dump()");
    i.DoString("m()");
    i.DoString("m2 = Matrix4f.New()");
    i.DoString("m3 = m * m2");
    i.DoString("m3:Dump()");
    //i.Globals().Set("TestFunction", i.Bind(&TestFunction));
    //i.DoString("TestFunction(10)");

    MetaTable mA(i, A());
    //auto bite5 = i.Bind(&A::Print, std::placeholders::_1, std::placeholders::_2);
    //auto r = i.Globals().Set("A", bite5);
    //r(A(), 50);


    MetaTable mVector2d(i, Tools::Vector2d());
    //i.Bind(&Tools::Vector2d::Normalize, std::placeholders::_1);
    //i.Bind(&Tools::Vector2d::Dot, std::placeholders::_1, std::placeholders::_2);

    //auto tmp = std::bind(&A::Print, std::placeholders::_1, std::placeholders::_2);
    //tmp(A(), 10);

    i.DumpStack();

#ifdef _WIN32
    std::cin.get();
#endif
    return 0;
}
