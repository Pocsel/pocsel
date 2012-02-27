#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Iterator.hpp"

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
    call.PushRet(call.GetInterpreter().MakeNumber(123.2));
    call.PushRet(call.GetInterpreter().MakeNumber(-5.4));
}

int main(int, char**)
{
    Tools::Lua::Interpreter i;

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

    i.DoString("test = \"sjkldglkhsdfg\" g = 323.4 jdsk = {} iwer = {} jkd = function() end");

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

    i.DumpStack();

    return 0;
}
