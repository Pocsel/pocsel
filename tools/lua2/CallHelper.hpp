#ifndef __TOOLS_LUA_CALLHELPER_HPP__
#define __TOOLS_LUA_CALLHELPER_HPP__

namespace Tools { namespace Lua {

    class Interpreter;
    class Ref;

    class CallHelper :
        private boost::noncopyable
    {
    private:
        Interpreter& _i;
        std::list<Ref> _args;
        std::list<Ref> _rets;

    public:
        CallHelper(Interpreter& i);
        Interpreter& GetInterpreter() { return this->_i; }
        // arguments
        void PushArg(Ref const& arg) throw();
        Ref PopArg() throw(std::runtime_error);
        std::list<Ref>& GetArgList() throw() { return this->_args; }
        std::size_t GetNbArgs() const throw() { return this->_args.size(); }
        void ClearArgs() throw();
        // return values
        void PushRet(Ref const& ret) throw();
        Ref PopRet() throw(std::runtime_error);
        std::list<Ref>& GetRetList() throw() { return this->_rets; }
        std::size_t GetNbRets() const throw() { return this->_rets.size(); }
        void ClearRets() throw();
    };

}}

#endif
