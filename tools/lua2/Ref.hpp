#ifndef __TOOLS_LUA_REF_HPP__
#define __TOOLS_LUA_REF_HPP__

#include "tools/lua2/CallHelper.hpp"
#include "tools/lua2/State.hpp"

namespace Tools { namespace Lua {

    class Ref
    {
    private:
        State& _state;
        int _ref;

    public:
        Ref(State& state) throw();
        Ref(Ref const& ref) throw();
        ~Ref() throw();
        Ref& operator =(Ref const& ref) throw();
        bool operator ==(Ref const& ref) const throw();
        bool operator !=(Ref const& ref) const throw();
        void Unref() throw();
        // function call
        void operator ()(CallHelper& callHelper) const throw(std::runtime_error);
        Ref operator ()() const throw(std::runtime_error);
        template <typename T>
            Ref operator ()(T a1) const throw(std::runtime_error);
        template <typename T, typename U>
            Ref operator ()(T a1, U a2) const throw(std::runtime_error);
        template <typename T, typename U, typename V>
            Ref operator ()(T a1, U a2, V a3) const throw(std::runtime_error);
        template <typename T, typename U, typename V, typename W>
            Ref operator ()(T a1, U a2, V a3, W a4) const throw(std::runtime_error);
        // array access
        Ref operator [](Ref const& index) const throw(std::runtime_error);
        template <typename T>
            Ref operator [](T const& index) const throw(std::runtime_error);
        // array setters
        void Set(Ref const& key, Ref const& value) const throw(std::runtime_error);
        template <typename T, typename U>
            void Set(T const& key, U const& value) const throw(std::runtime_error);
        // type conversions
        bool ToBoolean() const throw();
        int ToInteger() const throw();
        double ToNumber() const throw();
        std::string ToString() const throw();
        // type tests
        int GetType() const throw(); // valeurs possibles : LUA_TNIL, LUA_TBOOLEAN, LUA_TLIGHTUSERDATA, LUA_TNUMBER, LUA_TSTRING, LUA_TTABLE, LUA_TFUNCTION, LUA_TUSERDATA, LUA_TTHREAD, LUA_TNONE
        bool Exists() const throw(); // <- pareil que IsNoneOrNil(), utiliser ça en priorité plutôt que IsNil()/IsNoneOrNil()/IsNone()
        bool IsBoolean() const throw();
        bool IsCFunction() const throw();
        bool IsFunction() const throw();
        bool IsLightUserData() const throw();
        bool IsNil() const throw();
        bool IsNone() const throw();
        bool IsNoneOrNil() const throw();
        bool IsNumber() const throw();
        bool IsString() const throw();
        bool IsTable() const throw();
        bool IsThread() const throw();
        bool IsUserData() const throw();

        // touche pas, appelle pas
        void FromStack() throw();
        void ToStack() const throw();
    };

    inline Ref Ref::operator ()() const throw(std::runtime_error)
    {
        CallHelper callHelper(this->_state.GetInterpreter());
        (*this)(callHelper);
        if (callHelper.GetNbRets())
            return callHelper.PopRet();
        return Ref(*this);
    }

    template <typename T>
        Ref Ref::operator ()(T a1) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.Make(a1));
            (*this)(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T, typename U>
        Ref Ref::operator ()(T a1, U a2) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.Make(a1));
            callHelper.PushArg(this->_state.Make(a2));
            (*this)(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T, typename U, typename V>
        Ref Ref::operator ()(T a1, U a2, V a3) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.Make(a1));
            callHelper.PushArg(this->_state.Make(a2));
            callHelper.PushArg(this->_state.Make(a3));
            (*this)(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T, typename U, typename V, typename W>
        Ref Ref::operator ()(T a1, U a2, V a3, W a4) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.Make(a1));
            callHelper.PushArg(this->_state.Make(a2));
            callHelper.PushArg(this->_state.Make(a3));
            callHelper.PushArg(this->_state.Make(a4));
            (*this)(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T>
        Ref Ref::operator [](T const& index) const throw(std::runtime_error)
        {
            return (*this)[this->_state.Make(index)];
        }

    template <typename T, typename U>
        void Ref::Set(T const& key, U const& value) const throw(std::runtime_error)
        {
            this->Set(this->_state.Make(key), this->_state.Make(value));
        }
}}

#endif
