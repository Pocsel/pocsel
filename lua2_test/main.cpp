#include "tools/lua2/Interpreter.hpp"
#include "tools/lua2/Ref.hpp"

void f(Tools::Lua::Call& call)
{
    auto const& args = call.GetArgList();
    auto it = args.begin();
    auto itEnd = args.end();
    for (; it != itEnd; ++it)
        Tools::log << "arg: " << it->ToNumber() << Tools::endl;
    Tools::log << "f()" << Tools::endl;
    call.PushRet(call.GetInterpreter().MakeNumber(123.2));
    call.PushRet(call.GetInterpreter().MakeNumber(-5.4));
    throw std::runtime_error("OMg!");
}

int main(int, char**)
{
    Tools::Lua::Interpreter i;

    i.DoString("test = {} test[4] = 23.34");

    Tools::Lua::Ref bite = i.MakeFunction(std::bind(&f, std::placeholders::_1));

    Tools::Lua::Call call(i);
    try
    {
        bite(call);
    }
    catch (std::exception& e)
    {
        Tools::log << e.what() << Tools::endl;
    }
    auto const& rets = call.GetRetList();
    auto it = rets.begin();
    auto itEnd = rets.end();
    for (; it != itEnd; ++it)
        Tools::log << "ret: " << it->ToNumber() << Tools::endl;

    return 0;
}
