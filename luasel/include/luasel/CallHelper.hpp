#ifndef __LUASEL_CALLHELPER_HPP__
#define __LUASEL_CALLHELPER_HPP__

#include <list>
#include <string>
#include <stdexcept>

namespace Luasel {

    class Interpreter;
    class Ref;

    class CallHelper
    {
        private:
            Interpreter& _i;
            std::list<Ref> _args;
            std::list<Ref> _rets;

        public:
            CallHelper(Interpreter& i) throw();
            Interpreter& GetInterpreter() throw() { return this->_i; }
            // arguments
            void PushArg(Ref const& arg) throw();
            Ref PopArg(std::string const& error = "") throw(std::runtime_error);
            std::list<Ref>& GetArgList() throw() { return this->_args; }
            std::size_t GetNbArgs() const throw() { return this->_args.size(); }
            void ClearArgs() throw();
            // return values
            void PushRet(Ref const& ret) throw();
            Ref PopRet(std::string const& error = "") throw(std::runtime_error);
            std::list<Ref>& GetRetList() throw() { return this->_rets; }
            std::size_t GetNbRets() const throw() { return this->_rets.size(); }
            void ClearRets() throw();

            // arguments helpers
            template<class T>
                void PushArg(T const& arg) throw();
            template<class T>
                void PushArgMove(T&& arg) throw();

            // return helpers
            template<class T>
                void PushRet(T const& ret) throw();
            template<class T>
                void PushRetMove(T&& ret) throw();
    };

}

#endif
