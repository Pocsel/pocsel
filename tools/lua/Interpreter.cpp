#include "tools/lua/Lua.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Iterator.hpp"

namespace Tools { namespace Lua {

    Interpreter::Interpreter() throw(std::runtime_error) :
        _serializer(*this)
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

    Ref Interpreter::LoadString(std::string const& code) const throw(std::runtime_error)
    {
        if (luaL_loadstring(*this->_state, code.c_str()))
        {
            std::string e = "Lua::Interpreter: Cannot load string: ";
            e += lua_tostring(*this->_state, -1);
            lua_pop(*this->_state, 1);
            throw std::runtime_error(e);
        }
        Ref r(*this->_state);
        r.FromStack();
        return r;
    }

    void Interpreter::RegisterLib(LibId lib) throw(std::runtime_error)
    {
        char const* name;
        lua_CFunction func;
        switch (lib)
        {
            case Base:
                name = "";
                func = &luaopen_base;
                break;
            case Math:
                name = LUA_MATHLIBNAME;
                func = &luaopen_math;
                break;
            case Table:
                name = LUA_TABLIBNAME;
                func = &luaopen_table;
                break;
            case String:
                name = LUA_STRLIBNAME;
                func = &luaopen_string;
                break;
            case Io:
                name = LUA_IOLIBNAME;
                func = &luaopen_io;
                break;
            case Os:
                name = LUA_OSLIBNAME;
                func = &luaopen_os;
                break;
            case Debug:
                name = LUA_DBLIBNAME;
                func = &luaopen_debug;
                break;
            case Package:
                name = LUA_LOADLIBNAME;
                func = &luaopen_package;
                break;
            default:
                throw std::runtime_error("Lua::Interpreter: Lib not found");
        }
        lua_pushcfunction(*this->_state, func);
        lua_pushstring(*this->_state, name);
        if (lua_pcall(*this->_state, 1, 0, 0))
        {
            std::string e = "Lua::Interpreter: Cannot load lib: ";
            e += lua_tostring(*this->_state, -1);
            lua_pop(*this->_state, 1);
            throw std::runtime_error(e);
        }
        if (lib == Base)
        {
            auto oldGetmetatable = this->Globals()["getmetatable"];
            this->Globals().Set("getmetatable", this->MakeFunction(
                [oldGetmetatable](CallHelper& helper)
                {
                    auto obj = helper.GetArgList().front();
                    if (obj.IsUserData())
                        throw std::runtime_error("Lua::Interpreter: getmetatable: expected table");
                    oldGetmetatable(helper);
                }));
        }
    }

    void Interpreter::DoString(std::string const& code) const throw(std::runtime_error)
    {
        if (luaL_dostring(*this->_state, code.c_str()))
        {
            std::string e = "Lua::Interpreter: Cannot execute string: ";
            e += lua_tostring(*this->_state, -1);
            lua_pop(*this->_state, 1);
            throw std::runtime_error(e);
        }
    }

    void Interpreter::DoFile(std::string const& path) const throw(std::runtime_error)
    {
        if (luaL_dofile(*this->_state, path.c_str()))
        {
            std::string e = "Lua::Interpreter: Cannot execute file: ";
            e += lua_tostring(*this->_state, -1);
            lua_pop(*this->_state, 1);
            throw std::runtime_error(e);
        }
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
            Tools::log << "" << std::endl;
        }
        Tools::log << "------------------------------\n";
    }

}}
