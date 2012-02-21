#include "tools/lua2/Lua.hpp"
#include "tools/lua2/Interpreter.hpp"

namespace Tools { namespace Lua {

    Interpreter::Interpreter() throw(std::runtime_error)
    {
        this->_state = luaL_newstate();
        if (!this->_state)
            throw std::runtime_error("Lua::Interpreter: Not enough memory");
    }

    Interpreter::~Interpreter() throw()
    {
        lua_close(this->_state);
    }

    Interpreter::operator lua_State*() const throw()
    {
        return this->_state;
    }

    void Interpreter::DumpStack() const throw()
    {
        Tools::log << "------- Lua Stack Dump -------\n";
        for (int i = 1; i <= lua_gettop(*this); ++i)
        {
            int type = lua_type(*this, i);
            Tools::log << i << "\t | " << lua_typename(*this, type);
            switch (type)
            {
            case LUA_TSTRING:
                Tools::log << "\t | \"" << lua_tostring(*this, i) << "\"";
                break;
            case LUA_TBOOLEAN:
                Tools::log << "\t | " << lua_toboolean(*this, i);
                break;
            case LUA_TNUMBER:
                Tools::log << "\t | " << lua_tonumber(*this, i);
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
        if (luaL_dostring(*this, code.c_str()))
        {
            std::string e = "Lua::Interpreter: Cannot load string: ";
            e += lua_tostring(*this, 1);
            lua_pop(*this, 1);
            throw std::runtime_error(e);
        }
    }

    void Interpreter::DoFile(std::string const& path) throw(std::runtime_error)
    {
        if (luaL_dofile(*this, path.c_str()))
        {
            std::string e = "Lua::Interpreter: Cannot load file: ";
            e += lua_tostring(*this, 1);
            lua_pop(*this, 1);
            throw std::runtime_error(e);
        }
    }

}}
