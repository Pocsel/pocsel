#include "tools/lua/Lua.hpp"
#include "tools/lua/State.hpp"
#include "tools/lua/CallHelper.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Interpreter.hpp"

namespace {

    int _LuaCall(lua_State* state)
    {
        {
            Tools::Lua::State::ClosureEnv* env = static_cast<Tools::Lua::State::ClosureEnv*>(lua_touserdata(state, lua_upvalueindex(1)));
            assert(env && "lua call with no env upvalue");
            Tools::Lua::CallHelper callHelper(*env->i);
            while (lua_gettop(state))
            {
                Tools::Lua::Ref arg(env->i->GetState());
                arg.FromStack();
                callHelper.PushArg(arg);
            }
            try
            {
                (*env->f)(callHelper);
            }
            catch (std::exception& e)
            {
                lua_pushstring(state, e.what());
                goto error;
            }
            auto const& rets = callHelper.GetRetList();
            auto it = rets.rbegin();
            auto itEnd = rets.rend();
            for (; it != itEnd; ++it)
                it->ToStack();
            return static_cast<int>(callHelper.GetNbRets());
        }
        error:
            return lua_error(state);
    }

    int _LuaGarbageCollect(lua_State* state)
    {
        Tools::Lua::State::ClosureEnv* env = static_cast<Tools::Lua::State::ClosureEnv*>(lua_touserdata(state, -1));
        assert(env && "lua garbage collect with no user data");
        delete env->f;
        return 0;
    }

}

namespace Tools { namespace Lua {

    State::State(Interpreter& interpreter) throw(std::runtime_error) :
        _interpreter(interpreter)
    {
        this->_state = luaL_newstate();
        if (!this->_state)
            throw std::runtime_error("Lua::State: Not enough memory");
    }

    State::~State() throw()
    {
        lua_close(this->_state);
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

    template <>
        Ref State::Make<Ref>(Ref const& val) throw()
        {
            return val;
        }

}}