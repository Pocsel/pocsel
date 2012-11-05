#ifndef __LUASEL_REF_HPP__
#define __LUASEL_REF_HPP__

#include <luasel/CallHelper.hpp>
#include <luasel/State.hpp>

namespace Luasel {

    class Iterator;

    class Ref
    {
        private:
            State& _state;
            int _ref;

        public:
            Ref(State& state) throw();
            Ref(Ref const& ref) throw();
            ~Ref() throw(); // call Unref() to allow destruction after interpreter destruction without crashing
            Ref& operator =(Ref const& ref) throw();
            bool operator ==(Ref const& ref) const throw(); // this is a value comparison, not a reference comparison (uses lua_rawequal)
            bool operator !=(Ref const& ref) const throw(); // same
            template <typename T>
                bool Equals(T value) const throw(); // same
            void Unref() throw();
            bool IsValid() const throw(); // returns false if Unref() was called, or if the reference was created without copy (with a State)
            size_t GetLength() const throw(); // returns the length for a string, the size for a table (#), the number of bytes for a user data and 0 for everything else
            // function call
            void Call(CallHelper& callHelper) const throw(std::runtime_error);
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
            Ref GetTable(std::string const& name) const throw(std::runtime_error); // returns the table "name" (it is created if necessary)
            Iterator Begin() const throw(std::runtime_error);
            Iterator End() const throw(std::runtime_error); // iterators are equal ONLY if they are end iterators (ref.Begin() != ref.Begin())
            Ref operator [](Ref const& index) const throw(std::runtime_error);
            template <typename T>
                Ref operator [](T index) const throw(std::runtime_error);
            // array setters
            Ref Set(Ref const& key, Ref const& value) const throw(std::runtime_error); // returns value
            template <typename T, typename U>
                Ref Set(T key, U value) const throw(std::runtime_error); // returns a new reference to value
            // metatable
            Ref SetMetaTable(Ref const& table) const throw(std::runtime_error); // returns table
            bool HasMetaTable() const throw(); // might return true even if the metatable is not a table (but something else like a number)
            Ref GetMetaTable() const throw(); // returns a reference to nil if no metatable was found
            // safe type conversions
            bool ToBoolean() const throw(); // everything is true except nil and false (0 and "0" are true)
            int ToInteger() const throw(); // type conversion done by lua, returns 0 in case of error
            double ToNumber() const throw(); // type conversion done by lua, returns 0 in case of error
            std::string ToString() const throw(); // type conversion done by lua, returns an empty string in case of error
            void* ToUserData() const throw(); // returns a null pointer if it's not a user data
            template <typename T>
                T To() const throw();
            template <typename T>
                T To(T const& defaultValue) const throw();
            // unsafe type conversions
            bool CheckBoolean(std::string const& e = "") const throw(std::runtime_error);
            int CheckInteger(std::string const& e = "") const throw(std::runtime_error); // actually checks for a number, because an integer is a number for lua
            double CheckNumber(std::string const& e = "") const throw(std::runtime_error);
            std::string CheckString(std::string const& e = "") const throw(std::runtime_error);
            void* CheckUserData(std::string const& e = "") const throw(std::runtime_error);
            template <typename T>
                T Check(std::string const& e) const throw(std::runtime_error);
            template <typename T>
                T Check() const throw(std::runtime_error) { return this->Check<T>(""); }
            // type tests
            std::string GetTypeName() const throw();
            int GetType() const throw(); // possible values : LUA_TNIL, LUA_TBOOLEAN, LUA_TLIGHTUSERDATA, LUA_TNUMBER, LUA_TSTRING, LUA_TTABLE, LUA_TFUNCTION, LUA_TUSERDATA, LUA_TTHREAD, LUA_TNONE
            bool Exists() const throw(); // same as IsNoneOrNil(), use this instead of IsNil()/IsNoneOrNil()/IsNone()
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
            template <typename T>
                bool Is() const throw();

            // other stuff (don't call)
            void FromStack() throw();
            void ToStack() const throw();
            State& GetState() const throw() { return this->_state; }
    };

}

#endif
