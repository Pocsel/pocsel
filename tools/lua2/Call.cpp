#include "tools/lua2/Call.hpp"
#include "tools/lua2/Interpreter.hpp"

namespace Tools { namespace Lua {

    Call::Call(Interpreter& i) :
        _i(i)
    {
    }

    void Call::PushArg(Ref const& arg) throw()
    {
        this->_args.push_back(arg);
    }

    Ref Call::PopArg() throw(std::runtime_error)
    {
        Ref arg = this->_args.front();
        this->_args.pop_front();
        return arg;
    }

    void Call::PushRet(Ref const& ret) throw()
    {
        this->_rets.push_back(ret);
    }

    Ref Call::PopRet() throw(std::runtime_error)
    {
        Ref ret = this->_rets.front();
        this->_rets.pop_front();
        return ret;
    }

}}
