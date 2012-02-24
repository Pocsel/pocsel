#ifndef __TOOLS_LUA_ITERATOR_HPP__
#define __TOOLS_LUA_ITERATOR_HPP__

#include "tools/lua2/Ref.hpp"

namespace Tools { namespace Lua {

    class Iterator
    {
    private:
        Ref _table;
        Ref _key;
        Ref _value;
        bool _end;

    public:
        Iterator(Ref const& table, bool end) throw(std::runtime_error);
        Iterator(Iterator const& iterator) throw();
        Iterator& operator =(Iterator const& iterator) throw();
        bool operator ==(Iterator const& iterator) const throw();
        bool operator !=(Iterator const& iterator) const throw();
        Iterator& operator ++() throw(std::runtime_error);
        bool IsEnd() const throw() { return this->_end; }
        Ref const& GetTable() const throw() { return this->_table; }
        Ref const& GetKey() const throw() { return this->_key; }
        Ref const& GetValue() const throw() { return this->_value; }
    };

}}

#endif
