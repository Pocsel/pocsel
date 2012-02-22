#include "tools/lua2/Stack.hpp"
#include "tools/lua2/Interpreter.hpp"

namespace Tools { namespace Lua {

    Stack::Stack(Interpreter& i) throw() :
        _i(i)
    {
    }

    void Stack::PushArg(Ref const& arg) throw()
    {
        this->_args.push_back(arg);
    }

    Ref Stack::PopArg() throw(std::runtime_error)
    {
        Ref arg = this->_args.front();
        this->_args.pop_front();
        return arg;
    }

    void Stack::PushRet(Ref const& ret) throw()
    {
        this->_rets.push_back(ret);
    }

    Ref Stack::PopRet() throw(std::runtime_error)
    {
        Ref ret = this->_rets.front();
        this->_rets.pop_front();
        return ret;
    }

}}
