#ifndef __LUASEL_ITERATOR_HPP__
#define __LUASEL_ITERATOR_HPP__

#include <luasel/Ref.hpp>

namespace Luasel {

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
            bool operator ==(Iterator const& iterator) const throw(); // iterators are equal ONLY if they are end iterators
            bool operator !=(Iterator const& iterator) const throw();
            Iterator& operator ++() throw(std::runtime_error);
            bool IsEnd() const throw() { return this->_end; }
            Ref const& GetTable() const throw() { return this->_table; }
            Ref const& GetKey() const throw() { return this->_key; }
            Ref const& GetValue() const throw() { return this->_value; }
    };

}

#endif
