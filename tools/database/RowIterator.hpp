#ifndef __TOOLS_DATABASE_ROWITERATOR_HPP__
#define __TOOLS_DATABASE_ROWITERATOR_HPP__

namespace Tools { namespace Database {

    class ICursor;
    class IRow;
    class IValue;

    class RowIterator
    {
    private:
        ICursor& _curs;
        IRow* _row;

    public:
        RowIterator(ICursor& curs);
        operator bool();
        IRow& operator *();
        IValue& operator [](unsigned int column);
        void operator ++();
    };

}}

#endif
