#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Iterator.hpp"
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

void Init(Interpreter& i)
{
    auto n = i.MakeTable();
    auto metaTable = i.MakeTable();
    auto prototype = i.MakeTable();
    i.Globals().Set("Matrix4f", n);
    prototype.Set("Rotate", i.MakeFunction([](CallHelper& helper) { }));
    prototype.Set("Dump", i.MakeFunction(
        [](CallHelper& helper)
        {
            auto self = helper.PopArg();
            auto m = (Tools::Matrix4<float>*)self.CheckUserData();
            for (int i = 0; i < 4; ++i)
                Tools::log << m->m[i][0] << ", " << m->m[i][1] << ", " << m->m[i][2] << ", " << m->m[i][3] << "\n";
        }));
    metaTable.Set("__index", prototype);
    metaTable.Set("__gc", i.MakeFunction([](CallHelper& helper) { auto m = (Tools::Matrix4<float>*)helper.PopArg().CheckUserData(); m->~Matrix4<float>(); }));

    n.Set("New", i.MakeFunction(
        [metaTable](CallHelper& helper)
        {
            Tools::Matrix4<float>* matrix;
            auto table = helper.GetInterpreter().MakeUserData((void**)&matrix, sizeof(*matrix));
            *matrix = Tools::Matrix4<float>::identity;
            table.SetMetaTable(metaTable);
            helper.PushRet(table);
        }));
}

int main(int, char**)
{
    Tools::Lua::Interpreter i;
    i.RegisterLib(Tools::Lua::Interpreter::Base);
    i.RegisterLib(Tools::Lua::Interpreter::Math);

    i.DoString("test = {} test[4] = 23.34");

    Tools::Lua::Ref bite = i.MakeFunction(std::bind(&f, std::placeholders::_1));

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

    i.DumpStack();

    return 0;
}
