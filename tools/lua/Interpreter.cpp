#include "tools/lua/Lua.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Tools { namespace Lua {

    Interpreter::Interpreter() throw(std::runtime_error)
    {
        this->_state = new State(*this);
        this->_globals = new Ref(*this->_state);
        lua_pushvalue(*this->_state, LUA_GLOBALSINDEX);
        this->_globals->FromStack();
    }

    Interpreter::~Interpreter() throw()
    {
        Tools::Delete(this->_globals);
        Tools::Delete(this->_state);
    }

    Ref const& Interpreter::Globals() const throw()
    {
        return *this->_globals;
    }

    void Interpreter::DumpStack() const throw()
    {
        Tools::log << "------- Lua Stack Dump -------\n";
        Tools::log << "(size: " << lua_gettop(*this->_state) << ")\n";
        for (int i = 1; i <= lua_gettop(*this->_state); ++i)
        {
            int type = lua_type(*this->_state, i);
            Tools::log << i << "\t | " << lua_typename(*this->_state, type);
            switch (type)
            {
            case LUA_TSTRING:
                Tools::log << "\t | \"" << lua_tostring(*this->_state, i) << "\"";
                break;
            case LUA_TBOOLEAN:
                Tools::log << "\t | " << lua_toboolean(*this->_state, i);
                break;
            case LUA_TNUMBER:
                Tools::log << "\t | " << lua_tonumber(*this->_state, i);
                break;
            default:
                ;
            }
            Tools::log << "\n";
        }
        Tools::log << "------------------------------\n";
    }

    void Interpreter::DoString(std::string const& code) throw(std::runtime_error)
    {
        if (luaL_dostring(*this->_state, code.c_str()))
        {
            std::string e = "Lua::Interpreter: Cannot load string: ";
            e += lua_tostring(*this->_state, -1);
            lua_pop(*this->_state, 1);
            throw std::runtime_error(e);
        }
    }

    void Interpreter::DoFile(std::string const& path) throw(std::runtime_error)
    {
        if (luaL_dofile(*this->_state, path.c_str()))
        {
            std::string e = "Lua::Interpreter: Cannot load file: ";
            e += lua_tostring(*this->_state, -1);
            lua_pop(*this->_state, 1);
            throw std::runtime_error(e);
        }
    }

}}
