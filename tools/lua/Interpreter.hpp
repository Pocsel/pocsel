#ifndef __TOOLS_LUA_INTERPRETER_HPP__
#define __TOOLS_LUA_INTERPRETER_HPP__

#include "tools/lua/State.hpp"
#include "tools/lua/Ref.hpp"

extern "C" {
    struct luaL_Reg;
}

namespace Tools { namespace Lua {

    class Interpreter :
        private boost::noncopyable
    {
    public:
        enum LibId
        {
            Base, // print(), error(), _G, dofile(), select(), tostring(), unpack()...
            Math, // math.abs(), math.sqrt()...
            Table, // table.insert(), table.sort()...
            String, // string.len(), string.format(), string.find()...
            Io, // io.open(), io.read()...
            Os, // os.execute(), os.exit(), os.date()...
            Debug, // debug.sethook(), debug.debug()...
            Package, // module(), package.loadlib(), package.seeall()...
        };

    private:
        State* _state;
        Ref* _globals;

    public:
        Interpreter() throw(std::runtime_error);
        ~Interpreter() throw();
        std::string Serialize(Ref const& ref) const throw(std::runtime_error);
        Ref Deserialize(std::string const& string) const throw(std::runtime_error);
        // script loading
        Ref LoadString(std::string const& code) const throw(std::runtime_error);
        void RegisterLib(LibId lib) const throw(std::runtime_error);
        void DoString(std::string const& code) const throw(std::runtime_error);
        void DoFile(std::string const& path) const throw(std::runtime_error);
        // global table
        Ref const& Globals() const throw();
        // reference creators
        Ref MakeBoolean(bool val) throw() { return this->_state->MakeBoolean(val); }
        Ref MakeFunction(std::function<void(CallHelper&)> val) throw() { return this->_state->MakeFunction(val); }
        Ref MakeNil() throw() { return this->_state->MakeNil(); }
        Ref MakeInteger(int val) throw() { return this->_state->MakeInteger(val); }
        Ref MakeNumber(double val) throw() { return this->_state->MakeNumber(val); }
        Ref MakeString(std::string const& val) throw() { return this->_state->MakeString(val); }
        Ref MakeTable() throw() { return this->_state->MakeTable(); }
        Ref MakeUserData(void** data, size_t size) throw(std::runtime_error) { return this->_state->MakeUserData(data, size); }
        template <typename T>
            Ref Make(T val) throw() { return this->_state->Make(val); }
        // other stuff
        State& GetState() throw() { return *this->_state; }
        void DumpStack() const throw();
    private:
        std::string _SerializeSimpleValue(Ref const& ref) const throw(std::runtime_error);
        std::string _SerializeString(std::string const& string) const;
        std::string _Serialize(Ref const& ref, std::list<Ref>& tables, unsigned int level) const throw(std::runtime_error);
    };

}}

#endif
