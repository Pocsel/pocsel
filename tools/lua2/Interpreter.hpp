#ifndef __TOOLS_LUA_INTERPRETER_HPP__
#define __TOOLS_LUA_INTERPRETER_HPP__

#include "tools/lua2/Ref.hpp"
#include "tools/lua2/Call.hpp"

extern "C" {
    struct lua_State;
}

namespace Tools { namespace Lua {

    class Interpreter :
        private boost::noncopyable
    {
    public:
        struct ClosureEnv
        {
            Interpreter* i;
            std::function<void(Call&)>* f;
        };

    private:
        lua_State* _state;
        Ref* _globals;

    public:
        Interpreter() throw(std::runtime_error);
        ~Interpreter() throw();
        Ref const& Globals() const throw();
        Ref MakeInteger(int val) throw();
        Ref MakeNumber(double val) throw();
        Ref MakeString(std::string const& val) throw();
        Ref MakeBoolean(bool val) throw();
        Ref MakeTable() throw();
        Ref MakeFunction(std::function<void(Call&)> val) throw();
        void DoString(std::string const& code) throw(std::runtime_error);
        void DoFile(std::string const& path) throw(std::runtime_error);
        void DumpStack() const throw();
        operator lua_State*() const throw();
    };

}}

#endif
