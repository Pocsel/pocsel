#include <luasel/Lua.hpp>
#include <luasel/State.hpp>
#include <luasel/CallHelper.hpp>
#include <luasel/Ref.hpp>
#include <luasel/Interpreter.hpp>
#include <luasel/Iterator.hpp>
#include <luasel/MetaTable.hpp>

namespace {

    int _RealLuaCall(lua_State* state)
    {
        Luasel::State::ClosureEnv* env = static_cast<Luasel::State::ClosureEnv*>(lua_touserdata(state, lua_upvalueindex(1)));
        assert(env && "Lua call with no env upvalue");
        Luasel::CallHelper callHelper(*env->i);
        while (lua_gettop(state))
        {
            Luasel::Ref arg(env->i->GetState());
            arg.FromStack();
            callHelper.GetArgList().push_front(arg);
        }
        try
        {
            (*env->f)(callHelper);
        }
        catch (std::exception& e)
        {
            lua_pushstring(state, e.what());
            return -1;
        }
        auto const& rets = callHelper.GetRetList();
        if (!lua_checkstack(env->i->GetState(), (int)rets.size()))
        {
            lua_pushstring(state, "Luasel::State: Call error: insufficient Lua stack size for return values");
            return -1;
        }
        auto it = rets.rbegin();
        auto itEnd = rets.rend();
        for (; it != itEnd; ++it)
            it->ToStack();
        return static_cast<int>(callHelper.GetNbRets());
    }

    int _LuaCall(lua_State* state)
    {
        int ret = _RealLuaCall(state);
        if (ret == -1)
        {
            auto err = lua_tostring(state, -1);
            lua_pop(state, 1);
            return luaL_error(state, "%s", err);
        }
        return ret;
    }

    int _LuaGarbageCollect(lua_State* state)
    {
        Luasel::State::ClosureEnv* env = static_cast<Luasel::State::ClosureEnv*>(lua_touserdata(state, -1));
        assert(env && "Lua garbage collection with no user data");
        delete env->f;
        return 0;
    }

}

namespace Luasel {

    State::State(Interpreter& interpreter) throw(std::runtime_error) :
        _interpreter(interpreter),
        _weakId(1),
        _garbageCollectionEnabled(true)
    {
        this->_state = luaL_newstate();
        if (!this->_state)
            throw std::runtime_error("Luasel::State: Not enough memory to instantiate new interpreter");
        this->_weakTable = new Ref(this->MakeTable());
        auto metaWeakTable = this->MakeTable();
        metaWeakTable.Set("__mode", "v");
        this->_weakTable->SetMetaTable(metaWeakTable);
    }

    State::~State() throw()
    {
        delete this->_weakTable;
        for (auto it = this->_metaTables.begin(), ite = this->_metaTables.end(); it != ite; ++it)
            delete it->second;
        //this->_metaTables.clear(); // leak possible
        lua_close(this->_state);
    }

    void State::StopGarbageCollector()
    {
        lua_gc(this->_state, LUA_GCSTOP, 0);
        this->_garbageCollectionEnabled = false;
    }

    void State::RestartGarbageCollector()
    {
        lua_gc(this->_state, LUA_GCRESTART, 0);
        this->_garbageCollectionEnabled = true;
    }

    MetaTable& State::RegisterMetaTable(MetaTable&& metaTable, std::size_t hash) throw()
    {
        auto ptr = new MetaTable(std::move(metaTable));
        this->_metaTables.insert(std::make_pair(hash, ptr));
        return *ptr;
    }

    Ref State::MakeBoolean(bool val) throw()
    {
        lua_pushboolean(*this, val);
        Ref r(*this);
        r.FromStack();
        return r;
    }

    Ref State::MakeFunction(std::function<void(CallHelper&)> val) throw()
    {
#ifdef new
#undef new
#endif
        ClosureEnv* env = new (lua_newuserdata(*this, sizeof(ClosureEnv))) ClosureEnv;
        env->i = &this->_interpreter;
        env->f = new std::function<void(CallHelper&)>(val);
        lua_createtable(*this, 0, 1);
        lua_pushstring(*this, "__gc");
        lua_pushcfunction(*this, &_LuaGarbageCollect);
        lua_rawset(*this, -3);
        lua_setmetatable(*this, -2);
        lua_pushcclosure(*this, &_LuaCall, 1);
        Ref r(*this);
        r.FromStack();
        return r;
    }

    Ref State::MakeNil() throw()
    {
        lua_pushnil(*this);
        Ref r(*this);
        r.FromStack();
        return r;
    }

    Ref State::MakeInteger(int val) throw()
    {
        lua_pushinteger(*this, val);
        Ref r(*this);
        r.FromStack();
        return r;
    }

    Ref State::MakeNumber(double val) throw()
    {
        lua_pushnumber(*this, val);
        Ref r(*this);
        r.FromStack();
        return r;
    }

    Ref State::MakeString(std::string const& val) throw()
    {
        lua_pushstring(*this, val.c_str());
        Ref r(*this);
        r.FromStack();
        return r;
    }

    Ref State::MakeTable() throw()
    {
        lua_newtable(*this);
        Ref r(*this);
        r.FromStack();
        return r;
    }

    Ref State::MakeUserData(void** data, size_t size) throw(std::runtime_error)
    {
        *data = lua_newuserdata(*this, size);
        if (!*data)
            throw std::runtime_error("Luasel::State: Not enough memory to instantiate new user data");
        Ref r(*this);
        r.FromStack();
        return r;
    }

#define MAKE_MAKEMETHOD(type, make_func) \
    template <> \
        Ref State::Make<type>(type const& val) throw() \
        { \
            return this->make_func(val); \
        }

    MAKE_MAKEMETHOD(bool, MakeBoolean);
    MAKE_MAKEMETHOD(int, MakeInteger);
    MAKE_MAKEMETHOD(unsigned int, MakeNumber); // je ne suis pas sur
    MAKE_MAKEMETHOD(char, MakeInteger);
    MAKE_MAKEMETHOD(unsigned char, MakeInteger);
    MAKE_MAKEMETHOD(double, MakeNumber);
    MAKE_MAKEMETHOD(float, MakeNumber);
    MAKE_MAKEMETHOD(std::string, MakeString);
    MAKE_MAKEMETHOD(char const*, MakeString);
    MAKE_MAKEMETHOD(std::function<void(CallHelper&)>, MakeFunction);

    template<> Ref State::Make<Ref>(Ref const& val) throw()
    {
        return val;
    }

    MetaTable const& State::GetMetaTable(std::size_t hash) throw(std::runtime_error)
    {
        auto it = this->_metaTables.find(hash);
        if (it == this->_metaTables.end())
            throw std::runtime_error("Luasel::State: MetaTable not found");
        return *it->second;
    }

    Ref State::GetWeakReference(unsigned int id) const
    {
        return (*this->_weakTable)[id];
    }

    unsigned int State::GetWeakReference(Ref const& ref)
    {
        this->_weakTable->Set(this->_weakId, ref);
        return this->_weakId++;
    }
}
