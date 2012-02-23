#ifndef __TOOLS_LUA_INTERPRETER_HPP__
#define __TOOLS_LUA_INTERPRETER_HPP__

#include "tools/lua2/State.hpp"
#include "tools/lua2/Ref.hpp"

namespace Tools { namespace Lua {

    class Interpreter :
        private boost::noncopyable
    {
    private:
        State* _state;
        Ref* _globals;

    public:
        Interpreter() throw(std::runtime_error);
        ~Interpreter() throw();
        // script loading
        void DoString(std::string const& code) throw(std::runtime_error);
        void DoFile(std::string const& path) throw(std::runtime_error);
        // global table
        Ref const& Globals() const throw();
        // reference creators
        Ref MakeBoolean(bool val) throw() { return this->_state->MakeBoolean(val); }
        Ref MakeFunction(std::function<void(CallHelper&)> val) throw() { return this->_state->MakeFunction(val); }
        Ref MakeInteger(int val) throw() { return this->_state->MakeInteger(val); }
        Ref MakeNumber(double val) throw() { return this->_state->MakeNumber(val); }
        Ref MakeString(std::string const& val) throw() { return this->_state->MakeString(val); }
        Ref MakeTable() throw() { return this->_state->MakeTable(); }
        template <typename T>
            Ref Make(T const& val) throw() { return this->_state->Make(val); }
        // other stuff
        State& GetState() throw() { return *this->_state; }
        void DumpStack() const throw();
    };

}}

#endif
