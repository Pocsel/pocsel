#include "tools/lua2/Lua.hpp"
#include "tools/lua2/Ref.hpp"
#include "tools/lua2/Interpreter.hpp"

namespace Tools { namespace Lua {

    Ref::Ref(Interpreter& i) throw() :
        _i(i), _ref(LUA_NOREF)
    {
    }

    Ref::Ref(Ref const& ref) throw() :
        _i(ref._i), _ref(LUA_NOREF)
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, ref._ref);
        this->FromStack();
    }

    Ref::~Ref() throw()
    {
        this->Unref();
    }

    Ref& Ref::operator =(Ref const& ref) throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, ref._ref);
        this->FromStack();
        return *this;
    }

    bool Ref::operator ==(Ref const& ref) const throw()
    {
        return this->_ref == ref._ref;
    }

    bool Ref::operator !=(Ref const& ref) const throw()
    {
        return this->_ref != ref._ref;
    }

    Ref Ref::operator [](std::string const& name) const throw(std::runtime_error)
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
        if (!lua_istable(this->_i, -1))
            throw std::runtime_error("Lua::Ref: Indexing a value that is not a table");
        lua_pushstring(this->_i, name.c_str());
        lua_rawget(this->_i, -2);
        Ref ret(this->_i);
        ret.FromStack();
        lua_pop(this->_i, 1);
        return ret;
    }

    void Ref::Unref() throw()
    {
        luaL_unref(this->_i, LUA_REGISTRYINDEX, this->_ref);
        this->_ref = LUA_NOREF;
    }

    void Ref::FromStack() throw()
    {
        this->Unref();
        this->_ref = luaL_ref(this->_i, LUA_REGISTRYINDEX);
    }

    std::string Ref::ToString() const throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
        std::string ret = lua_tostring(this->_i, -1);
        lua_pop(this->_i, 1);
        return ret;
    }

    double Ref::ToNumber() const throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
        double ret = lua_tonumber(this->_i, -1);
        lua_pop(this->_i, 1);
        return ret;
    }

    int Ref::GetType() const throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
        int type = lua_type(this->_i, -1);
        lua_pop(this->_i, 1);
        return type;
    }

    bool Ref::IsNoneOrNil() const throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
        bool isNoneOrNil = lua_isnoneornil(this->_i, -1);
        lua_pop(this->_i, 1);
        return isNoneOrNil;
    }

}}
