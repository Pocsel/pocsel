#include "tools/lua/CallHelper.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"

namespace Tools { namespace Lua {

    CallHelper::CallHelper(Interpreter& i) throw() :
        _i(i)
    {
    }

    void CallHelper::PushArg(Ref const& arg) throw()
    {
        this->_args.push_front(arg);
    }

    Ref CallHelper::PopArg(std::string const& e /* = "" */) throw(std::runtime_error)
    {
        if (this->_args.empty())
        {
            if (!e.empty())
                throw std::runtime_error(e);
            else
                throw std::runtime_error("Lua::CallHelper: Missing argument");
        }
        Ref arg = this->_args.front();
        this->_args.pop_front();
        return arg;
    }

    void CallHelper::ClearArgs() throw()
    {
        this->_args.clear();
    }

    void CallHelper::PushRet(Ref const& ret) throw()
    {
        this->_rets.push_front(ret);
    }

    Ref CallHelper::PopRet() throw(std::runtime_error)
    {
        if (this->_rets.empty())
            throw std::runtime_error("Lua::CallHelper: Missing return value");
        Ref ret = this->_rets.front();
        this->_rets.pop_front();
        return ret;
    }

    void CallHelper::ClearRets() throw()
    {
        this->_rets.clear();
    }

}}
