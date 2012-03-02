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
        Ref Make(T val) throw(std::runtime_error);
        // other stuff
        State& GetState() throw() { return *this->_state; }
        void DumpStack() const throw();

        template<class T>
        Ref Bind(T function);
        template<class T, class TA1>
        Ref Bind(T function, TA1 arg1);
        template<class T, class TA1, class TA2>
        Ref Bind(T function, TA1 arg1, TA2 arg2);
        template<class T, class TA1, class TA2, class TA3>
        Ref Bind(T function, TA1 arg1, TA2 arg2, TA3 arg3);
    private:
        std::string _SerializeSimpleValue(Ref const& ref) const throw(std::runtime_error);
        std::string _SerializeString(std::string const& string) const;
        std::string _Serialize(Ref const& ref, std::list<Ref>& tables, unsigned int level) const throw(std::runtime_error);
    };

    template <typename T>
    inline Ref Interpreter::Make(T val) throw(std::runtime_error)
    {
        auto m = this->_state->GetMetaTable(typeid(T).hash_code());
        T* luaValue;
        auto r = this->_state->MakeUserData(reinterpret_cast<void**>(&luaValue), sizeof(T));
        *luaValue = val;
        r.SetMetaTable(m);
        return r;
    }
    template<> inline Ref Interpreter::Make<bool>(bool val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<int>(int val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<unsigned int>(unsigned int val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<char>(char val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<unsigned char>(unsigned char val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<double>(double val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<float>(float val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<std::string>(std::string val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<char const*>(char const* val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<std::function<void(CallHelper&)>>(std::function<void(CallHelper&)> val) throw(std::runtime_error) { return this->_state->Make(val); }
    template<> inline Ref Interpreter::Make<Ref>(Ref val) throw(std::runtime_error) { return this->_state->Make(val); }

}}

#include "tools/lua/Function.hpp"

#endif
