#ifndef __TOOLS_LUA_INTERPRETER_HPP__
#define __TOOLS_LUA_INTERPRETER_HPP__

#include "tools/lua2/Ref.hpp"

extern "C" {
    struct lua_State;
}

namespace Tools { namespace Lua {

    class Interpreter :
        private boost::noncopyable
    {
    private:
        lua_State* _state;

    public:
        Interpreter() throw(std::runtime_error);
        ~Interpreter() throw();
        Ref operator [](std::string const& name) throw();
        void DoString(std::string const& code) throw(std::runtime_error);
        void DoFile(std::string const& path) throw(std::runtime_error);
        void DumpStack() const throw();
        operator lua_State*() const throw();
    };

}}

#endif
