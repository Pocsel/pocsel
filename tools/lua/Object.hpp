#ifndef __TOOLS_LUA_OBJECT_HPP__
#define __TOOLS_LUA_OBJECT_HPP__

#include <functional>
#include <iosfwd>
#include <string>

#include "LuaState.hpp"
#include "ObjectIterator.hpp"
#include "StackRefPtr.hpp"
#include "Exception.hpp"

namespace Tools { namespace Lua {

    class Object
    {
    public:
        typedef std::function<void(Object&)> Setter;
        typedef ObjectIterator iterator;

    protected:
        StackRefPtr     _ref;
        Setter          _setter;

    public:
        Object(StackRefPtr& ref, Setter setter) :
            _ref(ref),
            _setter(setter)
        {}
        Object(Object const& o) :
            _ref(o._ref),
            _setter(0)
        {}
        Object(Object&& o) :
            _ref(std::move(o._ref)),
            _setter(std::move(o._setter))
        {}
        Object& operator =(Object&& o)
        {
            if (this != &o)
            {
                this->_ref = std::move(o._ref);
                this->_setter = std::move(o._setter);
            }
            return *this;
        }


        double ToNumber() const;
        std::string ToString() const;
        bool ToBoolean() const;

        std::string ForceString() const;

        double GetNumber() const { return this->ToNumber(); }
        std::string GetString() const { return this->ToString(); }
        bool GetBoolean() const {return this->ToBoolean(); }

        std::string GetType() const;
        int GetIndex() const;

        template<typename T> T as();
        template<typename T> operator T() { return this->as<T>(); }

        iterator begin();
        iterator end();

        Object& SetNumber(double nb);
        Object& SetString(std::string const& s);
        Object& SetBoolean(bool b);

        Object& CreateTable();
        template<typename T> Object& operator =(T val);
        Object operator [](std::string const& key);
        Object operator [](unsigned int i);

        /////////////////////////////////////////////////////////////////////
        // Object operator ()(Args...)
        Object operator ()()
        { _Push(); return _Call(0); }
        template<typename T1>
            Object operator ()(T1 t1)
            { _Push(); _api().push(t1); return _Call(1); }
        template<typename T1, typename T2>
            Object operator ()(T1 t1, T2 t2)
            { _Push(); _api().push(t1); _api().push(t2); return _Call(2); }
        template<typename T1, typename T2, typename T3>
            Object operator ()(T1 t1, T2 t2, T3 t3)
            { _Push(); _api().push(t1); _api().push(t2); _api().push(t3); return _Call(3); }
        template<typename T1, typename T2, typename T3, typename T4>
            Object operator ()(T1 t1, T2 t2, T3 t3, T4 t4)
            { _Push(); _api().push(t1); _api().push(t2); _api().push(t3); _api().push(t4); return _Call(4); }

        bool IsNone() const;
        bool IsNil() const;
        bool IsNoneOrNil() const;
        bool IsTable() const;
        bool IsNumber() const;
        bool IsBoolean() const;
        bool IsString() const;
        bool IsFunction() const;

        friend std::ostream& operator <<(std::ostream& out, Object const& o)
        {
            out << o.ForceString();
            return out;
        }

    private:
        LuaState& _api();
        void _Push();
        Object _Call(int p);
    };

}}

#endif
