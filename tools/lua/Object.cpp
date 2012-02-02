
#include <cassert>
#include <iostream>

#include "tools/ToString.hpp"
#include "tools/meta/StaticIf.hpp"
#include "tools/meta/Equals.hpp"
#include "tools/meta/IsConvertible.hpp"

#include "Exception.hpp"
#include "Setters.hpp"

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include "Object.hpp"
#include "StackRef.hpp"

#ifdef _MSC_VER
# pragma warning(disable: 4244 4800)
#endif

using namespace Tools::Lua;

namespace {

    struct TempValue
    {
        StackRefPtr const& ref;

        TempValue(StackRefPtr const& ref) : ref(ref)
        {
            ref->luaState.checkstack(1);
            ref->luaState.getref(ref->idx);
        }
        ~TempValue()
        {
            ref->luaState.pop(1);
        }
    };

}

LuaState& Object::_api()
{
    return _ref->luaState;
}

void Object::_Push()
{
    _ref->luaState.checkstack(1);
    _ref->luaState.getref(_ref->idx);
}

Object Object::_Call(int p)
{
    if (!_ref->luaState.isfunction(-1 - p))
        throw ValueError("Index " + Tools::ToString(-1 - p) + " is not a function");
    if (_api().pcall(p, 1, 0))
    {
        throw RuntimeError(
            "While calling a function: " + std::string(_api().tostring(-1))
        );
    }
    Object o = _api().to<Object>(-1);
    _api().pop(1);
    return o;
}

bool Object::IsNone() const { TempValue t(_ref); return _ref->luaState.isnone(-1); }
bool Object::IsNil() const { TempValue t(_ref); return _ref->luaState.isnil(-1); }
bool Object::IsNoneOrNil() const { TempValue t(_ref); return _ref->luaState.isnoneornil(-1); }
bool Object::IsTable() const { TempValue t(_ref); return _ref->luaState.istable(-1); }
bool Object::IsNumber() const { TempValue t(_ref); return _ref->luaState.isnumber(-1); }
bool Object::IsBoolean() const { TempValue t(_ref); return _ref->luaState.isboolean(-1); }
bool Object::IsString() const { TempValue t(_ref); return _ref->luaState.isstring(-1); }
bool Object::IsFunction() const { TempValue t(_ref); return _ref->luaState.isfunction(-1); }

double Object::ToNumber() const
{
    TempValue t(_ref);
    if (!_ref->luaState.isnumber(-1))
        throw TypeError("Not a number");
    return _ref->luaState.tonumber(-1);
}

std::string Object::ToString() const
{
    TempValue t(_ref);
    if (!_ref->luaState.isstring(-1))
        throw TypeError("Not a string");
    return std::string(_ref->luaState.tostring(-1));
}

bool Object::ToBoolean() const
{
    TempValue t(_ref);
    if (!_ref->luaState.isboolean(-1))
        throw TypeError("Not a boolean");
    return _ref->luaState.toboolean(-1);
}

std::string Object::ForceString() const
{
    if (this->IsNone())
        return "none";
    else if (this->IsNil())
        return "nil";
    else if (this->IsNumber())
        return Tools::ToString(this->GetNumber());
    else if (this->IsBoolean())
        return (this->ToBoolean() ? "true" : "false");
    else if (this->IsString())
        return this->ToString();
    else
    {
        TempValue t(_ref);
        return (
            "<" + this->GetType() + " at " +
            Tools::ToString(_ref->luaState.topointer(-1)) + ">"
        );
    }
}

Object& Object::SetNumber(double val)
{
    _ref->luaState.unref(_ref->idx);
    _ref->luaState.pushnumber(val);
    _ref->idx = _ref->luaState.ref();
    if (_setter)
        _setter(*this);
    return *this;
}

Object& Object::SetString(std::string const& val)
{
    _ref->luaState.unref(_ref->idx);
    _ref->luaState.pushstring(val.c_str());
    _ref->idx = _ref->luaState.ref();
    if (_setter)
        _setter(*this);
    return *this;
}

Object& Object::SetBoolean(bool val)
{
    _ref->luaState.unref(_ref->idx);
    _ref->luaState.pushboolean(val);
    _ref->idx = _ref->luaState.ref();
    if (_setter)
        _setter(*this);
    return *this;
}

Object Object::operator [](std::string const& key)
{
    TempValue t(_ref);
    if (!_ref->luaState.istable(-1))
        throw TypeError("Not subscriptable");
    _ref->luaState.pushstring(key.c_str()); // we keep the key
    StackRefPtr keyRef(new StackRef(_ref->luaState));

    _ref->luaState.pushstring(key.c_str());
    _ref->luaState.gettable(-2);
    StackRefPtr newRef(new StackRef(_ref->luaState));
    return Object(newRef, TableSetter(_ref, keyRef));
}

Object Object::operator [](unsigned int i)
{
    TempValue t(_ref);
    if (!_ref->luaState.istable(-1))
        throw TypeError("Not subscriptable");
    if (i == 0)
        throw ValueError("0 is not a valid index");

    _ref->luaState.rawgeti(-1, i);
    StackRefPtr newRef(new StackRef(_ref->luaState));
    return Object(newRef, ArraySetter(_ref, i));
}

int Object::GetIndex() const { return this->_ref->idx; }

std::string Object::GetType() const
{
    TempValue t(_ref);
    return _ref->luaState.typestring(-1);
}

Object& Object::CreateTable()
{
    _ref->luaState.unref(_ref->idx);
    _ref->luaState.newtable();
    _ref->idx = _ref->luaState.ref();
    if (_setter)
        _setter(*this);
    return *this;
}

template<typename T> T Object::as()
{
    TempValue t(_ref);
    auto r = _ref->luaState.to<T>(-1);
    return r;
}

template<typename T> Object& Object::operator =(T val)
{
    _ref->luaState.unref(_ref->idx);
    _ref->luaState.push(val);
    _ref->idx = _ref->luaState.ref();
    if (_setter)
        _setter(*this);
    return *this;
}

#define FORCE_INSTANCE(Type) \
    template Type Object::as<Type>(); \
    template Object& Object::operator =<Type>(Type val)

FORCE_INSTANCE(char);
FORCE_INSTANCE(int);
FORCE_INSTANCE(long);
FORCE_INSTANCE(unsigned char);
FORCE_INSTANCE(unsigned int);
FORCE_INSTANCE(unsigned long);
FORCE_INSTANCE(float);
FORCE_INSTANCE(double);
FORCE_INSTANCE(bool);
FORCE_INSTANCE(std::string);
FORCE_INSTANCE(char const*);

///////////////////////////////////////////////////////////////////////
// Object::iterator
Object::iterator Object::begin()
{
    TempValue t(_ref);
    if (!_ref->luaState.istable(-1))
        throw TypeError("Not subscriptable");
    Object::iterator it(_ref, false);
    return std::move(it);
}

Object::iterator Object::end()
{
    TempValue t(_ref);
    if (!_ref->luaState.istable(-1))
        throw TypeError("Not subscriptable");
    Object::iterator it(_ref, true);
    return it;
}

