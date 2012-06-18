#ifndef __TOOLS_LUA_STATE_HPP__
#define __TOOLS_LUA_STATE_HPP__

extern "C" {
    struct lua_State;
}

namespace Tools { namespace Lua {

    class Interpreter;
    class Ref;
    class CallHelper;

    class State :
        private boost::noncopyable
    {
    public:
        struct ClosureEnv
        {
            Interpreter* i;
            std::function<void(CallHelper&)>* f;
        };

    private:
        Interpreter& _interpreter;
        lua_State* _state;
        std::unordered_map<std::size_t, Ref*> _metaTables;
        Ref* _weakTable;
        Uint32 _weakId;
        bool _garbageCollectionEnabled;

    public:
        State(Interpreter& interpreter) throw(std::runtime_error);
        ~State() throw();
        bool GetGarbageCollectionEnabled() const { return this->_garbageCollectionEnabled; }
        void StopGarbageCollector();
        void RestartGarbageCollector();
        void RegisterMetaTable(Ref const& metaTable, std::size_t hash) throw();
        Ref MakeBoolean(bool val) throw();
        Ref MakeFunction(std::function<void(CallHelper&)> val) throw();
        Ref MakeNil() throw();
        Ref MakeInteger(int val) throw();
        Ref MakeNumber(double val) throw();
        Ref MakeString(std::string const& val) throw();
        Ref MakeTable() throw();
        Ref MakeUserData(void** data, size_t size) throw(std::runtime_error);
        template<typename T>
            Ref Make(T const& val) throw();
        operator lua_State*() const throw() { return this->_state; }
        Interpreter& GetInterpreter() throw() { return this->_interpreter; }
        Ref GetMetaTable(std::size_t hash) throw(std::runtime_error);
        Ref GetWeakReference(Uint32 id) const;
        Uint32 GetWeakReference(Ref const& ref);
    };

    template<> Ref State::Make<bool>(bool const& val) throw();
    template<> Ref State::Make<int>(int const& val) throw();
    template<> Ref State::Make<unsigned int>(unsigned int const& val) throw();
    template<> Ref State::Make<char>(char const& val) throw();
    template<> Ref State::Make<unsigned char>(unsigned char const& val) throw();
    template<> Ref State::Make<double>(double const& val) throw();
    template<> Ref State::Make<float>(float const& val) throw();
    template<> Ref State::Make<std::string>(std::string const& val) throw();
    template<> Ref State::Make<char const*>(char const* const& val) throw();
    template<> Ref State::Make<std::function<void(CallHelper&)>>(std::function<void(CallHelper&)> const& val) throw();

    template<> Ref State::Make<Ref>(Ref const& val) throw();

}}

#endif
