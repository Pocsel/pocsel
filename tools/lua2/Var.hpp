#ifndef __TOOLS_LUA_VAR_HPP__
#define __TOOLS_LUA_VAR_HPP__

namespace Tools { namespace Lua {

    class Interpreter;

    class Var
    {
    private:
        Interpreter& _i;
        int _ref;

    public:
        Var(Interpreter& i) throw();
        ~Var() throw();
        void FromGlobalTable(std::string const& name) throw();
        std::string ToString() const throw();
        double ToDouble() const throw();
        int GetType() const throw();
        bool IsNil() const throw();
    };

}}

#endif
