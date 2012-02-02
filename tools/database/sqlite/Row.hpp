#ifndef __TOOLS_DATABASE_SQLITE_ROW_HPP__
#define __TOOLS_DATABASE_SQLITE_ROW_HPP__

#include <sqlite3.h>

#include "tools/database/IRow.hpp"
#include "Value.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Row : public Tools::Database::IRow
    {
    private:
        sqlite3* _db;
        sqlite3_stmt* _stmt;
        Tools::Database::IValue** _values;
        unsigned int _column_count;

    public:
        Row(::sqlite3* db, ::sqlite3_stmt* stmt);
        virtual Tools::Database::IValue& operator[](unsigned int column);
        virtual ~Row();
    };

}}}

#endif
