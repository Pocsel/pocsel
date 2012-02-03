#include <sstream>
#include <cassert>

#include "tools/database/DatabaseError.hpp"
#include "tools/database/RowIterator.hpp"

#include "tools/database/sqlite/Statement.hpp"
#include "tools/database/sqlite/Cursor.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    Cursor::Cursor(::sqlite3* db) :
        _db(db), _stmt(0), _status(Tools::Database::Status::UNDEFINED), _row(0), _lastRowId(0), _pending(false)
    {
        assert(db != 0 && "Database pointer is NULL");
    }

    Cursor::Cursor(Cursor const& curs) :
        _db(curs._db), _stmt(0), _status(curs._status), _row(0), _lastRowId(0), _pending(false)
    {
    }

    Cursor& Cursor::operator =(Cursor const& curs)
    {
        if (this != &curs)
        {
            this->_db = curs._db;
            this->_stmt = curs._stmt;
            this->_status = curs._status;
            delete this->_row;
            this->_row = 0;
            this->_lastRowId = 0;
            this->_pending = false;
        }
        return *this;
    }

    Cursor::~Cursor()
    {
        if (this->_pending)
        {
            this->_status = this->_stmt->Step();
            this->_pending = false;
        }
        delete this->_row;
        this->_row = 0;
        delete this->_stmt;
        this->_stmt = 0;
    }

    Tools::Database::IStatement* Cursor::Prepare(char const* req)
    {
        assert(req != 0 && "Given request is null !");
        ::sqlite3_stmt* stmt;
        int res = ::sqlite3_prepare_v2(
            this->_db,     // Database handle
            req,           // SQL statement, UTF-8 encoded
            -1,            // Maximum length of zSql in bytes.
            &stmt,         // OUT: Statement handle
            0              // OUT: Pointer to unused portion of zSql
        );
        if (res != SQLITE_OK)
        {
            std::string msg = "Cannot prepare the request '" + std::string(req) + "': " + ::sqlite3_errmsg(this->_db);
            (void)::sqlite3_finalize(stmt);
            throw Tools::Database::DatabaseError(msg);
        }
        return new Statement(this->_db, stmt);
    }

    Tools::Database::IStatement& Cursor::Execute(char const* req)
    {
        if (this->_stmt != 0)
        {
            if (this->_pending)
            {
                this->_stmt->Step();
                this->_pending = false;
            }
            delete this->_stmt;
            this->_stmt = 0;
            delete this->_row;
            this->_row = 0;
            this->_lastRowId = 0;
        }
        this->_stmt = dynamic_cast<Statement*>(this->Prepare(req));
        assert(this->_stmt != 0);
        this->_pending = true;
        this->_status = Tools::Database::Status::UNDEFINED;
        return *this->_stmt;
    }

    Uint64 Cursor::LastRowId()
    {
        if (this->_pending)
        {
            this->_status = this->_stmt->Step();
            this->_lastRowId = ::sqlite3_last_insert_rowid(this->_db);
            this->_pending = false;
        }
        return this->_lastRowId;
    }

    Tools::Database::IRow& Cursor::FetchOne()
    {
        if (this->_stmt == 0)
            throw Tools::Database::DatabaseError("Cursor cannot fetch without any call to execute()");
        if (!this->HasData())
            throw Tools::Database::DatabaseError("No more data is available (use hasData() before calling fetch())");
        this->_status = Tools::Database::Status::UNDEFINED;
        if (this->_row == 0)
            this->_row = new Row(this->_db, this->_stmt->getSqlite3Stmt());
        return *this->_row;
    }

    bool Cursor::HasData()
    {
        if (this->_status == Tools::Database::Status::UNDEFINED)
            this->_status = this->_stmt->Step();
        if (this->_pending)
        {
            this->_pending = false;
            this->_lastRowId = ::sqlite3_last_insert_rowid(this->_db);
        }
        switch (this->_status)
        {
        case Tools::Database::Status::READY:
            return true;
        case Tools::Database::Status::DONE:
            break;
        case Tools::Database::Status::BUSY:
            throw Tools::Database::DatabaseError("Database is busy");
        default:
            std::stringstream ss;
            ss << "Unknown status code '" << this->_status << '\'';
            throw Tools::Database::DatabaseError(ss.str());
        }
        delete this->_stmt;
        this->_stmt = 0;
        delete this->_row;
        this->_row = 0;
        return false;
    }

    Tools::Database::RowIterator Cursor::FetchAll()
    {
        return Tools::Database::RowIterator(*this);
    }

}}}
