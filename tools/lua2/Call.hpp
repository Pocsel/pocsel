#ifndef __TOOLS_LUA_CALL_HPP__
#define __TOOLS_LUA_CALL_HPP__

#include "tools/lua2/Ref.hpp"

namespace Tools { namespace Lua {

    class Interpreter;

    class Call :
        private boost::noncopyable
    {
    private:
        Interpreter& _i;
        std::list<Ref> _args;
        std::list<Ref> _rets;

    public:
        Call(Interpreter& i);
        Interpreter& GetInterpreter() { return this->_i; }
        // arguments
        void PushArg(Ref const& arg) throw();
        Ref PopArg() throw(std::runtime_error);
        std::list<Ref> const& GetArgList() const throw() { return this->_args; }
        unsigned int GetNbArgs() const throw() { return this->_args.size(); }
        // return values
        void PushRet(Ref const& ret) throw();
        Ref PopRet() throw(std::runtime_error);
        std::list<Ref> const& GetRetList() const throw() { return this->_rets; }
        unsigned int GetNbRets() const throw() { return this->_rets.size(); }
    };

}}

#endif
