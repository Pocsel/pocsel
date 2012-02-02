#ifndef __TOOLS_LUA_OBJECTITERATOR_HPP__
#define __TOOLS_LUA_OBJECTITERATOR_HPP__

#include <utility>

#include "StackRefPtr.hpp"

namespace Tools { namespace Lua {

    class Object;

    struct ObjectIterator
    {
    private:
        StackRefPtr _key;
        StackRefPtr _value;
        StackRefPtr _ref;
        bool _isEnd;

    public:
        ObjectIterator(StackRefPtr& _ref, bool isEnd);
        ObjectIterator(ObjectIterator&& other);

        ObjectIterator& operator =(ObjectIterator&& other)
        {
            if (this != &other)
            {
                _key = std::move(other._key);
                _value = std::move(other._value);
                _ref = std::move(other._ref);
                _isEnd = other._isEnd;
            }
            return *this;
        }
        bool operator !=(ObjectIterator const& o) { return !(*this == o); }
        bool operator ==(ObjectIterator const& o)
        {
            return _isEnd == o._isEnd;
        }
        Object operator *();
        Object key();
        Object value();
        void operator ++();
    private:
        ObjectIterator(ObjectIterator const&);
        ObjectIterator& operator =(ObjectIterator const&);
    };

}}

#endif
