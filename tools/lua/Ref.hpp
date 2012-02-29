#ifndef __TOOLS_LUA_REF_HPP__
#define __TOOLS_LUA_REF_HPP__

#include "tools/lua/CallHelper.hpp"
#include "tools/lua/State.hpp"

namespace Tools { namespace Lua {

    class Iterator;

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
        size_t GetLength() throw(); // retourne la longueur pour une chaine, la taille pour un tableau (#), le nombre d'octets pour un user data et 0 pour le reste
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
        Iterator Begin() const throw(std::runtime_error);
        Iterator End() const throw(std::runtime_error); // un Iterator n'est égal à un autre uniquement si les 2 sont des Iterator de fin
        Ref operator [](Ref const& index) const throw(std::runtime_error);
        template <typename T>
            Ref operator [](T index) const throw(std::runtime_error);
        // array setters
        Ref Set(Ref const& key, Ref const& value) const throw(std::runtime_error); // retourne value
        template <typename T, typename U>
            Ref Set(T key, U value) const throw(std::runtime_error); // retourne une nouvelle reference vers value
        // metatable
        Ref SetMetaTable(Ref const& table) const throw(std::runtime_error); // retourne table
        bool HasMetaTable() const throw();
        Ref GetMetaTable() const throw(); // retourne une reference vers nil si pas de metatable
        // safe type conversions
        bool ToBoolean() const throw(); // true pour toute valeur differente de false ou nil (true pour 0 ou "0")
        int ToInteger() const throw(); // conversion de type par lua, retourne 0 en cas d'erreur
        double ToNumber() const throw(); // conversion de type par lua, retourne 0 en cas d'erreur
        std::string ToString() const throw(); // conversion de type par lua, retourne une chaine vide en cas d'erreur
        void* ToUserData() const throw(); // retourne un pointeur nul si ce n'est pas un userdata
        template <typename T>
            T To() const throw();
        template <typename T>
            T To(T const& defaultValue) const throw();
        // unsafe type conversions
        bool CheckBoolean() const throw(std::runtime_error);
        int CheckInteger() const throw(std::runtime_error); // en fait, ça vérifie si c'est un number parce que le type integer est un number en interne
        double CheckNumber() const throw(std::runtime_error);
        std::string CheckString() const throw(std::runtime_error);
        void* CheckUserData() const throw(std::runtime_error);
        template <typename T>
            T Check() const throw(std::runtime_error);
        // type tests
        std::string GetTypeName() const throw();
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
        State& GetState() const throw() { return this->_state; }
    };

    template<> bool Ref::To<bool>() const throw();
    template<> int Ref::To<int>() const throw();
    template<> unsigned int Ref::To<unsigned int>() const throw();
    template<> char Ref::To<char>() const throw();
    template<> unsigned char Ref::To<unsigned char>() const throw();
    template<> double Ref::To<double>() const throw();
    template<> float Ref::To<float>() const throw();
    template<> std::string Ref::To<std::string>() const throw();

    template<class T>
    inline T Ref::Check() const throw(std::runtime_error)
    {
        if (this->GetMetaTable() != this->_state.GetMetaTable(typeid(std::remove_pointer<T>::type).hash_code()))
            throw std::runtime_error(std::string("Lua::Ref: Value is not of ") + typeid(std::remove_pointer<T>::type).name() + " type");
        return reinterpret_cast<T>(this->CheckUserData());
    }
    template<> bool Ref::Check<bool>() const throw(std::runtime_error);
    template<> int Ref::Check<int>() const throw(std::runtime_error);
    template<> unsigned int Ref::Check<unsigned int>() const throw(std::runtime_error);
    template<> char Ref::Check<char>() const throw(std::runtime_error);
    template<> unsigned char Ref::Check<unsigned char>() const throw(std::runtime_error);
    template<> double Ref::Check<double>() const throw(std::runtime_error);
    template<> float Ref::Check<float>() const throw(std::runtime_error);
    template<> std::string Ref::Check<std::string>() const throw(std::runtime_error);

    template <typename T>
        inline T Ref::To(T const& defaultValue) const throw()
        {
            try
            {
                return this->Check<T>();
            }
            catch (std::exception& e)
            {
                return defaultValue;
            }
        }

    template <typename T>
        inline Ref Ref::operator ()(T a1) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.GetInterpreter().Make(a1));
            (*this)(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T, typename U>
        inline Ref Ref::operator ()(T a1, U a2) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.GetInterpreter().Make(a1));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a2));
            (*this)(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T, typename U, typename V>
        inline Ref Ref::operator ()(T a1, U a2, V a3) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.GetInterpreter().Make(a1));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a2));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a3));
            (*this)(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T, typename U, typename V, typename W>
        inline Ref Ref::operator ()(T a1, U a2, V a3, W a4) const throw(std::runtime_error)
        {
            CallHelper callHelper(this->_state.GetInterpreter());
            callHelper.PushArg(this->_state.GetInterpreter().Make(a1));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a2));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a3));
            callHelper.PushArg(this->_state.GetInterpreter().Make(a4));
            (*this)(callHelper);
            if (callHelper.GetNbRets())
                return callHelper.PopRet();
            return Ref(*this);
        }

    template <typename T>
        inline Ref Ref::operator [](T index) const throw(std::runtime_error)
        {
            return (*this)[this->_state.Make(index)];
        }

    template <typename T, typename U>
        inline Ref Ref::Set(T key, U value) const throw(std::runtime_error)
        {
            return this->Set(this->_state.Make(key), this->_state.Make(value));
        }

}}

#endif
