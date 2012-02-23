#include "tools/lua2/Lua.hpp"
#include "tools/lua2/Ref.hpp"

namespace Tools { namespace Lua {

    Ref::Ref(State& state) throw() :
        _state(state), _ref(LUA_NOREF)
    {
    }

    Ref::Ref(Ref const& ref) throw() :
        _state(ref._state), _ref(LUA_NOREF)
    {
        ref.ToStack();
        this->FromStack();
    }

    Ref::~Ref() throw()
    {
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

    Ref Ref::operator [](Ref const& index) const throw(std::runtime_error)
    {
        this->ToStack();
        if (!lua_istable(this->_state, -1))
        {
            lua_pop(this->_state, 1);
            throw std::runtime_error("Lua::Ref: Indexing a value that is not a table");
        }
        index.ToStack();
        lua_rawget(this->_state, -2);
        Ref ret(this->_state);
        ret.FromStack();
        lua_pop(this->_state, 1);
        return ret;
    }

    Ref Ref::operator [](int index) const throw(std::runtime_error)
    {
        return (*this)[this->_state.MakeInteger(index)];
    }

    Ref Ref::operator [](std::string const& index) const throw(std::runtime_error)
    {
        return (*this)[this->_state.MakeString(index)];
    }

    void Ref::operator ()(CallHelper& call) const throw(std::runtime_error)
    {
        this->ToStack();
        if (!lua_isfunction(this->_state, -1))
        {
            lua_pop(this->_state, 1);
            throw std::runtime_error("Lua::Ref: Calling a value that is not a function");
        }
        call.ClearRets();
        auto const& args = call.GetArgList();
        auto it = args.rbegin();
        auto itEnd = args.rend();
        for (; it != itEnd; ++it)
            it->ToStack();
        if (lua_pcall(this->_state, static_cast<int>(args.size()), LUA_MULTRET, 0))
        {
            std::string e = "Lua::Ref: Error in function call: ";
            e += lua_tostring(this->_state, -1);
            lua_pop(this->_state, 1);
            throw std::runtime_error(e);
        }
        while (lua_gettop(this->_state))
        {
            Ref ret(this->_state);
            ret.FromStack();
            call.PushRet(ret);
        }
    }

    void Ref::Unref() throw()
    {
        luaL_unref(this->_state, LUA_REGISTRYINDEX, this->_ref);
        this->_ref = LUA_NOREF;
    }

    void Ref::FromStack() throw()
    {
        this->Unref();
        this->_ref = luaL_ref(this->_state, LUA_REGISTRYINDEX);
    }

#define MAKE_TOMETHOD(name, lua_func, type) \
    type Ref::name() const throw() \
    { \
        this->ToStack(); \
        type ret = lua_func(this->_state, -1); \
        lua_pop(this->_state, 1); \
        return ret; \
    }

    MAKE_TOMETHOD(ToBoolean, lua_toboolean, bool);
    MAKE_TOMETHOD(ToInteger, lua_tointeger, int);
    MAKE_TOMETHOD(ToNumber, lua_tonumber, double);
    MAKE_TOMETHOD(ToString, lua_tostring, std::string);

    int Ref::GetType() const throw()
    {
        this->ToStack();
        int type = lua_type(this->_state, -1);
        lua_pop(this->_state, 1);
        return type;
    }

    bool Ref::Exists() const throw()
    {
        return this->IsNoneOrNil();
    }

#define MAKE_ISMETHOD(name, lua_func) \
    bool Ref::name() const throw() \
    { \
        this->ToStack(); \
        bool ret = lua_func(this->_state, -1); \
        lua_pop(this->_state, -1); \
        return ret; \
    }

    MAKE_ISMETHOD(IsBoolean, lua_isboolean);
    MAKE_ISMETHOD(IsCFunction, lua_iscfunction);
    MAKE_ISMETHOD(IsFunction, lua_isfunction);
    MAKE_ISMETHOD(IsLightUserData, lua_islightuserdata);
    MAKE_ISMETHOD(IsNil, lua_isnil);
    MAKE_ISMETHOD(IsNone, lua_isnone);
    MAKE_ISMETHOD(IsNoneOrNil, lua_isnoneornil);
    MAKE_ISMETHOD(IsNumber, lua_isnumber);
    MAKE_ISMETHOD(IsString, lua_isstring);
    MAKE_ISMETHOD(IsTable, lua_istable);
    MAKE_ISMETHOD(IsThread, lua_isthread);
    MAKE_ISMETHOD(IsUserData, lua_isuserdata);

    void Ref::ToStack() const throw()
    {
        lua_rawgeti(this->_state, LUA_REGISTRYINDEX, this->_ref);
    }

}}
