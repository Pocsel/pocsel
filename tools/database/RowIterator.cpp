#include "tools/database/ICursor.hpp"
#include "tools/database/IRow.hpp"
#include "tools/database/IValue.hpp"
#include "tools/database/RowIterator.hpp"

namespace Tools { namespace Database {

    RowIterator::RowIterator(ICursor& curs) : _curs(curs), _row(0) {}
    RowIterator::operator bool() { return this->_curs.HasData(); }
    IRow& RowIterator::operator *()
    {
        if (!this->_row)
            this->_row = &this->_curs.FetchOne();
        return *this->_row;
    }
    IValue& RowIterator::operator [](unsigned int column)
    {
        if (!this->_row)
            this->_row = &this->_curs.FetchOne();
        return (*this->_row)[column];
    }

    void RowIterator::operator ++()
    {
        if (!this->_row)
            this->_curs.FetchOne();
        this->_row = &this->_curs.FetchOne();
    }

}}
