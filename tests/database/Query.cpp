#include "tools/precompiled.hpp"

#include "Query.hpp"
#include "Row.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    Query::Query(Connection& connection, std::string const& request)
        : _connection(connection),
        _bindIdx(1)
    {
        int ret = sqlite3_prepare_v2(this->_connection.GetSqliteHandler(), request.c_str(), request.length(), &this->_stmt, 0);
        if (ret != SQLITE_OK)
            throw std::runtime_error(std::string("Sqlite: Query error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
    }

    Query::~Query()
    {
        int ret = sqlite3_finalize(this->_stmt);
        if (ret != SQLITE_OK)
            throw std::runtime_error(std::string("Sqlite: ~Query error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
    }

    IQuery& Query::Bind(char val)
    {
        return this->Bind((int)val);
    }

    IQuery& Query::Bind(unsigned char val)
    {
        return this->Bind((int)val);
    }

    IQuery& Query::Bind(short val)
    {
        return this->Bind((int)val);
    }

    IQuery& Query::Bind(unsigned short val)
    {
        return this->Bind((int)val);
    }

    IQuery& Query::Bind(int val)
    {
        int ret = sqlite3_bind_int(this->_stmt, this->_bindIdx++, val);
        if (ret != SQLITE_OK)
            throw std::runtime_error(std::string("Sqlite: Bind error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
        return *this;
    }

    IQuery& Query::Bind(unsigned int val)
    {
        return this->Bind((int)val);
    }

    IQuery& Query::Bind(long long val)
    {
        int ret = sqlite3_bind_int64(this->_stmt, this->_bindIdx++, val);
        if (ret != SQLITE_OK)
            throw std::runtime_error(std::string("Sqlite: Bind error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
        return *this;
    }

    IQuery& Query::Bind(unsigned long long val)
    {
        return this->Bind((long long)val);
    }

    IQuery& Query::Bind(float val)
    {
        return this->Bind((double)val);
    }

    IQuery& Query::Bind(double val)
    {
        int ret = sqlite3_bind_double(this->_stmt, this->_bindIdx++, val);
        if (ret != SQLITE_OK)
            throw std::runtime_error(std::string("Sqlite: Bind error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
        return *this;
    }

    IQuery& Query::Bind(std::string const& val)
    {
        int ret = sqlite3_bind_text(this->_stmt, this->_bindIdx++, val.c_str(), val.length(), SQLITE_TRANSIENT);
        if (ret != SQLITE_OK)
            throw std::runtime_error(std::string("Sqlite: Bind error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
        return *this;
    }

    IQuery& Query::Bind(void const* tab, std::size_t size)
    {
        if (size > (std::size_t)std::numeric_limits<int>::max())
            throw std::overflow_error("Sqlite: Bind error: size overflow");
        int ret = sqlite3_bind_blob(this->_stmt, this->_bindIdx++, tab, (int)size, SQLITE_TRANSIENT);
        if (ret != SQLITE_OK)
            throw std::runtime_error(std::string("Sqlite: Bind error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
        return *this;
    }

    IQuery& Query::ExecuteNonSelect()
    {
        int ret = sqlite3_step(this->_stmt);
        if (ret != SQLITE_DONE)
            throw std::runtime_error(std::string("Sqlite: ExecuteNonSelect error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
        return *this;
    }

    std::unique_ptr<IRow> Query::Fetch()
    {
        int ret = sqlite3_step(this->_stmt);
        if (ret != SQLITE_DONE && ret != SQLITE_ROW)
            throw std::runtime_error(std::string("Sqlite: Fetch error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
        if (ret == SQLITE_DONE)
            return std::unique_ptr<IRow>((IRow*)0);
        return std::unique_ptr<IRow>(new Row(this->_connection, *this));
    }

    void Query::Reset()
    {
        int ret = sqlite3_reset(this->_stmt);
        if (ret != SQLITE_OK)
            throw std::runtime_error(std::string("Sqlite: Reset error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
        ret = sqlite3_clear_bindings(this->_stmt);
        if (ret != SQLITE_OK)
            throw std::runtime_error(std::string("Sqlite: Reset error: ") + sqlite3_errmsg(this->_connection.GetSqliteHandler()));
        this->_bindIdx = 1;
    }

}}}
