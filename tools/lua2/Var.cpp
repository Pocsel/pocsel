#include "tools/lua2/Lua.hpp"
#include "tools/lua2/Var.hpp"
#include "tools/lua2/Interpreter.hpp"

namespace Tools { namespace Lua {

    Var::Var(Interpreter& i) throw() :
        _i(i), _ref(LUA_NOREF)
    {
    }

    Var::~Var() throw()
    {
        luaL_unref(this->_i, LUA_REGISTRYINDEX, this->_ref);
    }

    void Var::FromGlobalTable(std::string const& name) throw()
    {
        luaL_unref(this->_i, LUA_REGISTRYINDEX, this->_ref);
        lua_getglobal(this->_i, name.c_str());
        this->_ref = luaL_ref(this->_i, LUA_REGISTRYINDEX);
    }

    std::string Var::ToString() const throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
        char const* ret = lua_tostring(this->_i, 1);
        lua_pop(this->_i, 1);
        return ret;
    }

    double Var::ToDouble() const throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
        double ret = lua_tonumber(this->_i, 1);
        lua_pop(this->_i, 1);
        return ret;
    }

    int Var::GetType() const throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
        int type = lua_type(this->_i, 1);
        lua_pop(this->_i, 1);
        return type;
    }

    bool Var::IsNil() const throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
        bool isNil = lua_isnil(this->_i, 1);
        lua_pop(this->_i, 1);
        return isNil;
    }

}}
