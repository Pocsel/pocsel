#ifndef __TOOLS_LUA_INTERPRETER_HPP__
#define __TOOLS_LUA_INTERPRETER_HPP__

#include <boost/noncopyable.hpp>
#include <string>
#include <list>

#include "LuaState.hpp"
#include "Object.hpp"

namespace Tools { namespace Lua {

    struct Callable
    {
        virtual void operator()(LuaState&) = 0;
        virtual ~Callable() {}
    };

    class Interpreter : boost::noncopyable
    {
    private:
        std::list<Callable*> _bound;

    public:
        LuaState api;

    public:
        Interpreter(std::string const& code);
        Interpreter();
        ~Interpreter();
        Object operator [] (std::string const& key);
        Object operator [] (char const* key);
        void Load(std::string const& code);
        void Exec(std::string const& code);
        void ExecFile(std::string const& path);
        /////////////////////////////////////////////////////////////////////
        // Object Bind(Func, Args...)
        template<typename Func>
            Object Bind(Func func);
        template<typename Func, typename T1>
            Object Bind(Func func, T1 t1);
        template<typename Func, typename T1, typename T2>
            Object Bind(Func func, T1 t1, T2 t2);
        template<typename Func, typename T1, typename T2, typename T3>
            Object Bind(Func func, T1 t1, T2 t2, T3 t3);
        template<typename Func, typename T1, typename T2, typename T3, typename T4>
            Object Bind(Func func, T1 t1, T2 t2, T3 t3, T4 t4);

        /////////////////////////////////////////////////////////////////////
        // Object Bind(char const*, Func, Args...)
        template<typename Func>
            Object Bind(char const* str, Func func);
        template<typename Func, typename T1>
            Object Bind(char const* str, Func func, T1 t1);
        template<typename Func, typename T1, typename T2>
            Object Bind(char const* str, Func func, T1 t1, T2 t2);
        template<typename Func, typename T1, typename T2, typename T3>
            Object Bind(char const* str, Func func, T1 t1, T2 t2, T3 t3);
        template<typename Func, typename T1, typename T2, typename T3, typename T4>
            Object Bind(char const* str, Func func, T1 t1, T2 t2, T3 t3, T4 t4);

        /////////////////////////////////////////////////////////////////////
        // Object Bind(std::string const&, Func, Args...)
        template<typename Func>
            Object Bind(std::string const& str, Func func)
            { return Bind<Func>(str.c_str(), func); }
        template<typename Func, typename T1>
            Object Bind(std::string const& str, Func func, T1 t1)
            { return Bind<Func, T1>(str.c_str(), func, t1); }
        template<typename Func, typename T1, typename T2>
            Object Bind(std::string const& str, Func func, T1 t1, T2 t2)
            { return Bind<Func, T1, T2>(str.c_str(), func, t1, t2); }
        template<typename Func, typename T1, typename T2, typename T3>
            Object Bind(std::string const& str, Func func, T1 t1, T2 t2, T3 t3)
            { return Bind<Func, T1, T2, T3>(str.c_str(), func, t1, t2, t3); }
        template<typename Func, typename T1, typename T2, typename T3, typename T4>
            Object Bind(std::string const& str, Func func, T1 t1, T2 t2, T3 t3, T4 t4)
            { return Bind<Func, T1, T2, T3, T4>(str.c_str(), func, t1, t2, t3, t4); }

        template<typename TLogger>
            void DumpStack(TLogger& logger)
            {
                logger << "------------------------------- Dump stack\n";
                int top = api.gettop();
                for (int i = 1; i <= top; i++)
                {
                    logger << i << ": " << api.typestring(i) << ": ";
                    if (api.isnumber(i)) logger << api.tonumber(i);
                    else if (api.isstring(i)) logger << api.tostring(i);
                    else if (api.isboolean(i)) logger << (api.toboolean(i) ? "true" : "false");
                    else logger << api.topointer(i);
                    logger << '\n';
                }
                logger << "------------------------------- top = " << top << "\n";
            }

        void DumpStack() { this->DumpStack(std::cout); }

    private:
        template<typename Sig, typename Func>
            void _Bind(Func f);
    };

}}

#endif
