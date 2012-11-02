#include <luasel/Lua.hpp>
#include <luasel/Ref.hpp>
#include <luasel/Iterator.hpp>
#include <luasel/Interpreter.hpp>

#ifdef _MSC_VER
#pragma warning(disable: 4244) // return conversion double to float (data lost)
#pragma warning(disable: 4800) // int to bool
#endif

namespace Luasel {

    Ref::Ref(State& state) throw() :
        _state(state), _ref(LUA_NOREF)
    {
    }

    Ref::Ref(Ref const& ref) throw() :
        _state(ref._state), _ref(LUA_NOREF)
    {
        *this = ref;
    }

    Ref::~Ref() throw()
    {
        this->Unref();
    }

    Ref& Ref::operator =(Ref const& ref) throw()
    {
        if (this != &ref)
        {
            ref.ToStack();
            this->FromStack();
        }
        return *this;
    }

    bool Ref::operator ==(Ref const& ref) const throw()
    {
        ref.ToStack();
        this->ToStack();
        bool equal = lua_rawequal(this->_state, -1, -2);
        lua_pop(this->_state, 2);
        return equal;
    }

    bool Ref::operator !=(Ref const& ref) const throw()
    {
        return !(*this == ref);
    }

    void Ref::Unref() throw()
    {
        if (this->_ref == LUA_NOREF)
            return;
        luaL_unref(this->_state, LUA_REGISTRYINDEX, this->_ref);
        this->_ref = LUA_NOREF;
    }

    bool Ref::IsValid() const throw()
    {
        return this->_ref != LUA_NOREF;
    }

    size_t Ref::GetLength() const throw()
    {
        this->ToStack();
        size_t ret = lua_objlen(this->_state, -1);
        lua_pop(this->_state, 1);
        return ret;
    }

    Ref Ref::operator ()() const throw(std::runtime_error)
    {
        CallHelper callHelper(this->_state.GetInterpreter());
        this->Call(callHelper);
        if (callHelper.GetNbRets())
            return callHelper.PopRet();
        return Ref(*this);
    }

    void Ref::Call(CallHelper& call) const throw(std::runtime_error)
    {
        this->ToStack();
        if (!lua_isfunction(this->_state, -1))
        {
            lua_pop(this->_state, 1);
            throw std::runtime_error("Luasel::Ref: Calling a value that is not a function");
        }
        call.ClearRets();
        auto const& args = call.GetArgList();
        if (!lua_checkstack(this->_state, (int)args.size()))
        {
            lua_pop(this->_state, 1);
            throw std::runtime_error("Luasel::Ref: Insufficient Lua stack size for arguments");
        }
        auto it = args.rbegin();
        auto itEnd = args.rend();
        for (; it != itEnd; ++it)
            it->ToStack();
        if (lua_pcall(this->_state, static_cast<int>(args.size()), LUA_MULTRET, 0))
        {
            std::string e = "Luasel::Ref: Error in function call: ";
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

    Ref Ref::GetTable(std::string const& name) const throw(std::runtime_error)
    {
        auto table = this->operator[](name);
        if (!table.Exists())
            table = this->Set(name, this->_state.MakeTable());
        if (!table.IsTable())
            throw std::runtime_error("Luasel::Ref: Indexing a value that is not a table");
        return table;
    }

    Iterator Ref::Begin() const throw(std::runtime_error)
    {
        return Iterator(*this, false);
    }

    Iterator Ref::End() const throw(std::runtime_error)
    {
        return Iterator(*this, true);
    }

    Ref Ref::operator [](Ref const& index) const throw(std::runtime_error)
    {
        this->ToStack();
        if (!lua_istable(this->_state, -1))
        {
            lua_pop(this->_state, 1);
            throw std::runtime_error("Luasel::Ref: Indexing a value that is not a table");
        }
        index.ToStack();
        if (lua_isnoneornil(this->_state, -1))
        {
            lua_pop(this->_state, 2);
            throw std::runtime_error("Luasel::Ref: Using nil as an index");
        }
        lua_rawget(this->_state, -2);
        Ref ret(this->_state);
        ret.FromStack();
        lua_pop(this->_state, 1);
        return ret;
    }

    Ref Ref::Set(Ref const& key, Ref const& value) const throw(std::runtime_error)
    {
        this->ToStack();
        if (!lua_istable(this->_state, -1))
        {
            lua_pop(this->_state, 1);
            throw std::runtime_error("Luasel::Ref: Indexing a value that is not a table");
        }
        key.ToStack();
        if (lua_isnoneornil(this->_state, -1))
        {
            lua_pop(this->_state, 2);
            throw std::runtime_error("Luasel::Ref: Using nil as an index");
        }
        value.ToStack();
        lua_rawset(this->_state, -3);
        lua_pop(this->_state, 1);
        return value;
    }

    Ref Ref::SetMetaTable(Ref const& table) const throw(std::runtime_error)
    {
        this->ToStack();
        table.ToStack();
        if (!lua_istable(this->_state, -1))
        {
            lua_pop(this->_state, 2);
            throw std::runtime_error("Luasel::Ref: Setting metatable to a value that is not a table");
        }
        lua_setmetatable(this->_state, -2);
        lua_pop(this->_state, 1);
        return table;
    }

    bool Ref::HasMetaTable() const throw()
    {
        this->ToStack();
        if (lua_getmetatable(this->_state, -1))
        {
            lua_pop(this->_state, 2);
            return true;
        }
        lua_pop(this->_state, 1);
        return false;
    }

    Ref Ref::GetMetaTable() const throw()
    {
        this->ToStack();
        if (lua_getmetatable(this->_state, -1))
        {
            Ref ret(this->_state);
            ret.FromStack();
            lua_pop(this->_state, 1);
            return ret;
        }
        lua_pop(this->_state, 1);
        return Ref(this->_state);
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
    MAKE_TOMETHOD(ToUserData, lua_touserdata, void*);

    std::string Ref::ToString() const throw()
    {
        this->ToStack();
        char const* str = lua_tostring(this->_state, -1);
        if (str)
        {
            std::string ret(str);
            lua_pop(this->_state, 1);
            return ret;
        }
        lua_pop(this->_state, 1);
        return std::string();
    }

#define MAKE_TOTEMPLATEMETHOD(method, type) \
    template <> \
        type Ref::To<type>() const throw() \
        { \
            return this->method(); \
        }

    MAKE_TOTEMPLATEMETHOD(ToBoolean, bool);
    MAKE_TOTEMPLATEMETHOD(ToInteger, int);
    MAKE_TOTEMPLATEMETHOD(ToNumber, unsigned int); // je ne suis pas sur
    MAKE_TOTEMPLATEMETHOD(ToInteger, char);
    MAKE_TOTEMPLATEMETHOD(ToInteger, unsigned char);
    MAKE_TOTEMPLATEMETHOD(ToNumber, double);
    MAKE_TOTEMPLATEMETHOD(ToNumber, float);
    MAKE_TOTEMPLATEMETHOD(ToString, std::string);

#define MAKE_CHECKMETHOD(name, lua_checkfunc, lua_tofunc, type, error) \
    type Ref::name(std::string const& e) const throw(std::runtime_error) \
    { \
        this->ToStack(); \
        if (!lua_checkfunc(this->_state, -1)) \
        { \
            lua_pop(this->_state, 1); \
            if (!e.empty()) \
                throw std::runtime_error(e); \
            else \
                throw std::runtime_error(error); \
        } \
        type ret = lua_tofunc(this->_state, -1); \
        lua_pop(this->_state, 1); \
        return ret; \
    }

    MAKE_CHECKMETHOD(CheckBoolean, lua_isboolean, lua_toboolean, bool, "Luasel::Ref: Value is not of boolean type");
    MAKE_CHECKMETHOD(CheckInteger, lua_isnumber, lua_tointeger, int, "Luasel::Ref: Value is not of integer type");
    MAKE_CHECKMETHOD(CheckNumber, lua_isnumber, lua_tonumber, double, "Luasel::Ref: Value is not of number type");
    MAKE_CHECKMETHOD(CheckUserData, lua_isuserdata, lua_touserdata, void*, "Luasel::Ref: Value is not of user data type");

    std::string Ref::CheckString(std::string const& e) const throw(std::runtime_error)
    {
        this->ToStack();
        if (!lua_isstring(this->_state, -1))
        {
            lua_pop(this->_state, 1);
            if (!e.empty())
                throw std::runtime_error(e);
            else
                throw std::runtime_error("Luasel::Ref: Value is not of string type");
        }
        char const* str = lua_tostring(this->_state, -1);
        if (str)
        {
            std::string ret(str);
            lua_pop(this->_state, 1);
            return ret;
        }
        lua_pop(this->_state, 1);
        return std::string();
    }

#define MAKE_CHECKTEMPLATEMETHOD(method, type) \
    template <> \
        type Ref::Check<type>(std::string const& e) const throw(std::runtime_error) \
        { \
            return this->method(e); \
        }

    MAKE_CHECKTEMPLATEMETHOD(CheckBoolean, bool);
    MAKE_CHECKTEMPLATEMETHOD(CheckInteger, int);
    MAKE_CHECKTEMPLATEMETHOD(CheckNumber, unsigned int); // je ne suis pas sur
    MAKE_CHECKTEMPLATEMETHOD(CheckInteger, char);
    MAKE_CHECKTEMPLATEMETHOD(CheckInteger, unsigned char);
    MAKE_CHECKTEMPLATEMETHOD(CheckNumber, double);
    MAKE_CHECKTEMPLATEMETHOD(CheckNumber, float);
    MAKE_CHECKTEMPLATEMETHOD(CheckString, std::string);

    std::string Ref::GetTypeName() const throw()
    {
        return lua_typename(this->_state, this->GetType());
    }

    int Ref::GetType() const throw()
    {
        this->ToStack();
        int type = lua_type(this->_state, -1);
        lua_pop(this->_state, 1);
        return type;
    }

    bool Ref::Exists() const throw()
    {
        return !this->IsNoneOrNil();
    }

#define MAKE_ISMETHOD(name, lua_func) \
    bool Ref::name() const throw() \
    { \
        this->ToStack(); \
        bool ret = lua_func(this->_state, -1); \
        lua_pop(this->_state, 1); \
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

    void Ref::FromStack() throw()
    {
        this->Unref();
        this->_ref = luaL_ref(this->_state, LUA_REGISTRYINDEX);
    }

    void Ref::ToStack() const throw()
    {
        lua_rawgeti(this->_state, LUA_REGISTRYINDEX, this->_ref);
    }

}
