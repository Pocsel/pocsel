#include "tools/lua2/Lua.hpp"
#include "tools/lua2/Interpreter.hpp"

namespace {

    int _LuaCall(lua_State* state)
    {
        int* data = static_cast<int*>(lua_touserdata(state, lua_upvalueindex(1)));
        Tools::log << "userdata: " << *data << Tools::endl;
        return 0;
    }

}

namespace Tools { namespace Lua {

    Interpreter::Interpreter() throw(std::runtime_error)
    {
        this->_state = luaL_newstate();
        if (!this->_state)
            throw std::runtime_error("Lua::Interpreter: Not enough memory");
        this->_globals = new Ref(*this);
        lua_pushvalue(*this, LUA_GLOBALSINDEX);
        this->_globals->FromStack();
    }

    Interpreter::~Interpreter() throw()
    {
        Tools::Delete(this->_globals);
        Tools::log << "lua_close" << Tools::endl;
        lua_close(this->_state);
    }

    Interpreter::operator lua_State*() const throw()
    {
        return this->_state;
    }

    Ref const& Interpreter::Globals() const throw()
    {
        return *this->_globals;
    }

    Ref Interpreter::MakeInteger(int val) throw()
    {
        Ref r(*this);
        lua_pushinteger(*this, val);
        r.FromStack();
        return r;
    }

    Ref Interpreter::MakeNumber(double val) throw()
    {
        Ref r(*this);
        lua_pushnumber(*this, val);
        r.FromStack();
        return r;
    }

    Ref Interpreter::MakeString(std::string const& val) throw()
    {
        Ref r(*this);
        lua_pushstring(*this, val.c_str());
        r.FromStack();
        return r;
    }

    Ref Interpreter::MakeBoolean(bool val) throw()
    {
        Ref r(*this);
        lua_pushboolean(*this, val);
        r.FromStack();
        return r;
    }

    Ref Interpreter::MakeTable() throw()
    {
        Ref r(*this);
        lua_newtable(*this);
        r.FromStack();
        return r;
    }

    Ref Interpreter::MakeFunction(std::function<void(Stack&)> val) throw()
    {
        Ref r(*this);
        int* data = static_cast<int*>(lua_newuserdata(*this, sizeof(int)));
        *data = 1334;
        lua_pushcclosure(*this, &_LuaCall, 1);
        r.FromStack();
        return r;
    }

    void Interpreter::DumpStack() const throw()
    {
        Tools::log << "------- Lua Stack Dump -------\n";
        Tools::log << "(size: " << lua_gettop(*this) << ")\n";
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
            e += lua_tostring(*this, -1);
            lua_pop(*this, 1);
            throw std::runtime_error(e);
        }
    }

    void Interpreter::DoFile(std::string const& path) throw(std::runtime_error)
    {
        if (luaL_dofile(*this, path.c_str()))
        {
            std::string e = "Lua::Interpreter: Cannot load file: ";
            e += lua_tostring(*this, -1);
            lua_pop(*this, 1);
            throw std::runtime_error(e);
        }
    }

}}
