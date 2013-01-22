#ifndef __TOOLS_LUA_ITERATOR_HPP__
#define __TOOLS_LUA_ITERATOR_HPP__

#include "tools/lua/Ref.hpp"

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
        bool operator ==(Iterator const& iterator) const throw(); // un Iterator n'est égal à un autre uniquement si les 2 sont des Iterator de fin
        bool operator !=(Iterator const& iterator) const throw();
        Iterator& operator ++() throw(std::runtime_error);
        bool IsEnd() const throw() { return this->_end; }
        Ref const& GetTable() const throw() { return this->_table; }
        Ref const& GetKey() const throw() { return this->_key; }
        Ref const& GetValue() const throw() { return this->_value; }
    };

}}

#endif
