#include "tools/precompiled.hpp"

#include "tools/database/DatabaseError.hpp"
#include "tools/database/sqlite/Statement.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    Statement::Statement(::sqlite3* db, ::sqlite3_stmt* stmt) : _db(db), _stmt(stmt), _index(1)
    {
        assert(this->_stmt != 0 && "Statement is NULL");
    }

    Statement::~Statement()
    {
        ::sqlite3_finalize(this->_stmt);
    }

    Tools::Database::IStatement& Statement::Bind(int val)
    {
        int res = ::sqlite3_bind_int(this->_stmt, this->_index, val);
        if (res != SQLITE_OK)
            this->_raiseError(val);
        this->_index += 1;
        return *this;
    }

    Tools::Database::IStatement& Statement::Bind(unsigned int val)
    {
        int res = ::sqlite3_bind_int(this->_stmt, this->_index, val);
        if (res != SQLITE_OK)
            this->_raiseError(val);
        this->_index += 1;
        return *this;
    }

    Tools::Database::IStatement& Statement::Bind(Int64 val)
    {
        int res = ::sqlite3_bind_int64(this->_stmt, this->_index, val);
        if (res != SQLITE_OK)
            this->_raiseError(val);
        this->_index += 1;
        return *this;
    }

    Tools::Database::IStatement& Statement::Bind(Uint64 val)
    {
        int res = ::sqlite3_bind_int64(this->_stmt, this->_index, val);
        if (res != SQLITE_OK)
            this->_raiseError(val);
        this->_index += 1;
        return *this;
    }

    Tools::Database::IStatement& Statement::Bind(float val)
    {
        int res = ::sqlite3_bind_double(this->_stmt, this->_index, val);
        if (res != SQLITE_OK)
            this->_raiseError(val);
        this->_index += 1;
        return *this;
    }

    Tools::Database::IStatement& Statement::Bind(double val)
    {
        int res = ::sqlite3_bind_double(this->_stmt, this->_index, val);
        if (res != SQLITE_OK)
            this->_raiseError(val);
        this->_index += 1;
        return *this;
    }

    Tools::Database::IStatement& Statement::Bind(bool val)
    {
        int res = ::sqlite3_bind_int(this->_stmt, this->_index, val);
        if (res != SQLITE_OK)
            this->_raiseError(val);
        this->_index += 1;
        return *this;
    }

    Tools::Database::IStatement& Statement::Bind(std::string const& s)
    {
        int res = ::sqlite3_bind_text(this->_stmt, this->_index, s.c_str(), -1, SQLITE_TRANSIENT);
        if (res != SQLITE_OK)
            this->_raiseError(s);
        this->_index += 1;
        return *this;
    }

    Tools::Database::IStatement& Statement::Bind(const char* s)
    {
        int res = ::sqlite3_bind_text(this->_stmt, this->_index, s, -1, SQLITE_TRANSIENT);
        if (res != SQLITE_OK)
            this->_raiseError(s);
        this->_index += 1;
        return *this;
    }

    Tools::Database::IStatement& Statement::Bind(Tools::Database::Blob& val)
    {
        int res = ::sqlite3_bind_blob(this->_stmt, this->_index, val.data, static_cast<int>(val.size), SQLITE_STATIC);
        if (res != SQLITE_OK)
        {
            std::stringstream ss;
            ss << "Cannot bind blob of size '" << val.size
                << "' to index " + this->_index
                << ": " << ::sqlite3_errmsg(this->_db);
            throw Tools::Database::DatabaseError(ss.str());
        }
        this->_index += 1;
        return *this;
    }

    void Statement::Reset()
    {
        ::sqlite3_reset(this->_stmt);
    }

    Tools::Database::Status::Type Statement::Step()
    {
        int res = ::sqlite3_step(this->_stmt);
        switch (res)
        {
            case SQLITE_DONE:
                return Tools::Database::Status::DONE;
            case SQLITE_BUSY:
                return Tools::Database::Status::BUSY;
            case SQLITE_ROW:
                return Tools::Database::Status::READY;
            default:
                throw Tools::Database::DatabaseError(
                        "Cannot go forward: " +
                        std::string(::sqlite3_errmsg(this->_db))
                        );
        }
    }

    template<typename T>
    void Statement::_raiseError(T val)
    {
        std::stringstream ss;
        ss << "Cannot bind value '" << val
            << "' to index " + this->_index
            << ": " << ::sqlite3_errmsg(this->_db);
        throw Tools::Database::DatabaseError(ss.str());
    }

    template void Statement::_raiseError(int);
    template void Statement::_raiseError(unsigned int);
    template void Statement::_raiseError(Int64);
    template void Statement::_raiseError(Uint64);
    template void Statement::_raiseError(float);
    template void Statement::_raiseError(double);
    template void Statement::_raiseError(bool);

}}}
