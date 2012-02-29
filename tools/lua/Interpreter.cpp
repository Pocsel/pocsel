#include "tools/lua/Lua.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Ref.hpp"
#include "tools/lua/Iterator.hpp"

namespace Tools { namespace Lua {

    Interpreter::Interpreter() throw(std::runtime_error)
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

    std::string Interpreter::Serialize(Ref const& ref) const throw(std::runtime_error)
    {
        std::list<Ref> tables;
        return "return " + this->_Serialize(ref, tables, 1);
    }

    Ref Interpreter::Deserialize(std::string const& string) const throw(std::runtime_error)
    {
        if (string.empty())
            return Ref(*this->_state);
        return this->LoadString(string)();
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

    void Interpreter::RegisterLib(LibId lib) const throw(std::runtime_error)
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
            Tools::log << "\n";
        }
        Tools::log << "------------------------------\n";
    }

    std::string Interpreter::_SerializeSimpleValue(Ref const& ref) const throw(std::runtime_error)
    {
        if (ref.IsNumber())
            return Tools::ToString(ref.ToNumber());
        else if (ref.IsBoolean())
            return ref.ToBoolean() ? "true" : "false";
        else if (ref.IsString())
            return this->_SerializeString(ref.ToString());
        else if (ref.IsNil())
            return "nil";
        throw std::runtime_error("Lua::Interpreter: Type " + ref.GetTypeName() + " is not serializable");
    }

    std::string Interpreter::_SerializeString(std::string const& string) const
    {
        std::string ret;
        ret += '"';
        auto it = string.begin();
        auto itEnd = string.end();
        for (; it != itEnd; ++it)
        {
            unsigned char c = *it;
            if (c < ' ' || c > '~' || c == '"' || c == '\\')
            {
                ret += '\\';
                if (c < 100)
                    ret += '0';
                if (c < 10)
                    ret += '0';
                ret += Tools::ToString(static_cast<unsigned int>(c));
            }
            else
                ret += c;
        }
        ret += '"';
        return ret;
    }

    std::string Interpreter::_Serialize(Ref const& ref, std::list<Ref>& tables, unsigned int level) const throw(std::runtime_error)
    {
        if (ref.IsTable())
        {
            auto findIt = std::find(tables.begin(), tables.end(), ref);
            if (findIt != tables.end())
                throw std::runtime_error("Lua::Interpreter: Cyclic or shared table(s) found in table to serialize");
            tables.push_front(ref);
            std::string ret = "{\n";
            auto it = ref.Begin();
            auto itEnd = ref.End();
            for (; it != itEnd; ++it)
            {
                for (unsigned int i = 0; i < level; ++i)
                    ret += "\t";
                ret += "[" + this->_SerializeSimpleValue(it.GetKey()) + "] = " + this->_Serialize(it.GetValue(), tables, level + 1) + ",\n";
            }
            for (unsigned int i = 0; i < level - 1; ++i)
                ret += "\t";
            ret += "}";
            return ret;
        }
        else
            return this->_SerializeSimpleValue(ref);
    }

}}
