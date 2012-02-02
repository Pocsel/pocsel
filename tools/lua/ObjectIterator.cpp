
#include <iostream>
#include <cassert>

#include "ObjectIterator.hpp"
#include "StackRef.hpp"
#include "Exception.hpp"
#include "Object.hpp"
#include "Setters.hpp"

using namespace Tools::Lua;

ObjectIterator::ObjectIterator(StackRefPtr& ref, bool isEnd) :
    _key(),
    _value(),
    _ref(ref),
    _isEnd(isEnd)
{
    if (!isEnd)
    {
        auto& api = _ref->luaState;
        api.checkstack(3);
        api.getref(_ref->idx);
        assert(api.istable(-1));
        api.pushnil();
        if (!_ref->luaState.next(-2))
        {
            _isEnd = true;
        }
        else
        {
            _value.reset(new StackRef(_ref->luaState));
            _key.reset(new StackRef(_ref->luaState));
        }
        _ref->luaState.pop(1); //pop table
    }
}

ObjectIterator::ObjectIterator(ObjectIterator&& other) :
    _key(std::move(other._key)),
    _value(std::move(other._value)),
    _ref(std::move(other._ref)),
    _isEnd(other._isEnd)
{}

Object ObjectIterator::operator*()
{
    assert(!_isEnd);
    return Object(_value, TableSetter(_ref, _key));
}

Object ObjectIterator::value()
{
    assert(!_isEnd);
    return Object(_value, TableSetter(_ref, _key));
}

Object ObjectIterator::key()
{
    assert(!_isEnd);
    return Object(_key, 0);
}

void ObjectIterator::operator ++()
{
    assert(!_isEnd);
    auto& api = _ref->luaState;
    api.checkstack(3);
    api.getref(_ref->idx);
    assert(api.istable(-1));

    _ref->luaState.getref(_key->idx);
    assert(!api.isnil(-1));
    if (!_ref->luaState.next(-2))
    {
        _isEnd = true;
        _value.reset();
        _key.reset();
    }
    else
    {
        _value = StackRefPtr(new StackRef(_ref->luaState));
        _key = StackRefPtr(new StackRef(_ref->luaState));
    }
    _ref->luaState.pop(1); // pop table
}
