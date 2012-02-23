#ifndef __TOOLS_LUA_REF_HPP__
#define __TOOLS_LUA_REF_HPP__

namespace Tools { namespace Lua {

    class Interpreter;
    class Call;

    class Ref
    {
    private:
        Interpreter& _i;
        int _ref;

    public:
        Ref(Interpreter& i) throw();
        Ref(Ref const& ref) throw();
        ~Ref() throw();
        // operators
        Ref& operator =(Ref const& ref) throw();
        bool operator ==(Ref const& ref) const throw();
        bool operator !=(Ref const& ref) const throw();
        Ref operator [](Ref const& ref) const throw(std::runtime_error);
        void operator ()(Call& call) const throw(std::runtime_error);
        // modifiers
        void Unref() throw();
        void FromStack() throw();
        void Set(Ref const& key, Ref const& value) throw(std::runtime_error);
        // type conversions
        std::string ToString() const throw();
        double ToNumber() const throw();
        // type tests
        int GetType() const throw();
        bool IsNoneOrNil() const throw();

        void ToStack() const throw();
    };

}}

#endif
