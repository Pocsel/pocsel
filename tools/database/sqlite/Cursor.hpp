#ifndef __TOOLS_DATABASE_SQLITE_CURSOR_HPP__
#define __TOOLS_DATABASE_SQLITE_CURSOR_HPP__

#include <sqlite3.h>

#include "tools/database/status.hpp"
#include "tools/database/ICursor.hpp"

#include "tools/database/sqlite/Statement.hpp"
#include "tools/database/sqlite/Row.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Cursor : public Tools::Database::ICursor
    {
    private:
        ::sqlite3* _db;
        Statement* _stmt;
        Tools::Database::Status::Type _status;
        Tools::Database::IRow* _row;
        unsigned int _count;
        Uint64 _lastRowId;
        bool _pending;

    public:
        Cursor(::sqlite3* db);
        Cursor(Cursor const& curs);
        Cursor& operator =(Cursor const& curs);
        virtual ~Cursor();
        virtual Tools::Database::IStatement& Execute(char const* req);
        virtual Tools::Database::IStatement* Prepare(char const* req);
        virtual Tools::Database::IRow& FetchOne();
        virtual Uint64 LastRowId();
        virtual Tools::Database::RowIterator FetchAll();
        virtual bool HasData();
    };

}}}

#endif
