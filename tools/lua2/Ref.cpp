#include "tools/lua2/Lua.hpp"
#include "tools/lua2/Ref.hpp"
#include "tools/lua2/Interpreter.hpp"
#include "tools/lua2/Call.hpp"

namespace Tools { namespace Lua {

    Ref::Ref(Interpreter& i) throw() :
        _i(i), _ref(LUA_NOREF)
    {
    }

    Ref::Ref(Ref const& ref) throw() :
        _i(ref._i), _ref(LUA_NOREF)
    {
        ref.ToStack();
        this->FromStack();
    }

    Ref::~Ref() throw()
    {
        Tools::log << "~Ref()" << Tools::endl;
        this->Unref();
    }

    Ref& Ref::operator =(Ref const& ref) throw()
    {
        ref.ToStack();
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

    Ref Ref::operator [](Ref const& ref) const throw(std::runtime_error)
    {
        this->ToStack();
        if (!lua_istable(this->_i, -1))
        {
            lua_pop(this->_i, 1);
            throw std::runtime_error("Lua::Ref: Indexing a value that is not a table");
        }
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, ref._ref);
        lua_rawget(this->_i, -2);
        Ref ret(this->_i);
        ret.FromStack();
        lua_pop(this->_i, 1);
        return ret;
    }

    void Ref::operator ()(Call& call) const throw(std::runtime_error)
    {
        this->ToStack();
        if (!lua_isfunction(this->_i, -1))
        {
            lua_pop(this->_i, 1);
            throw std::runtime_error("Lua::Ref: Calling a value that is not a function");
        }
        auto const& args = call.GetArgList();
        auto it = args.begin();
        auto itEnd = args.end();
        for (; it != itEnd; ++it)
            it->ToStack();
        if (lua_pcall(this->_i, static_cast<int>(args.size()), LUA_MULTRET, 0))
        {
            std::string e = "Lua::Ref: Error in function call: ";
            e += lua_tostring(this->_i, -1);
            lua_pop(this->_i, 1);
            throw std::runtime_error(e);
        }
        while (lua_gettop(this->_i))
        {
            Ref ret(this->_i);
            ret.FromStack();
            call.PushRet(ret);
        }
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
        this->ToStack();
        std::string ret = lua_tostring(this->_i, -1);
        lua_pop(this->_i, 1);
        return ret;
    }

    double Ref::ToNumber() const throw()
    {
        this->ToStack();
        double ret = lua_tonumber(this->_i, -1);
        lua_pop(this->_i, 1);
        return ret;
    }

    int Ref::GetType() const throw()
    {
        this->ToStack();
        int type = lua_type(this->_i, -1);
        lua_pop(this->_i, 1);
        return type;
    }

    bool Ref::IsNoneOrNil() const throw()
    {
        this->ToStack();
        bool isNoneOrNil = lua_isnoneornil(this->_i, -1);
        lua_pop(this->_i, 1);
        return isNoneOrNil;
    }

    void Ref::ToStack() const throw()
    {
        lua_rawgeti(this->_i, LUA_REGISTRYINDEX, this->_ref);
    }

}}
