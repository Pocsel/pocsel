#include "tools/precompiled.hpp"

#include <sqlite3.h>
#include "Connection.hpp"
#include "Query.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    Connection::Connection(std::string const& settings)
    {
        if (sqlite3_open(settings.c_str(), &this->_sqliteDb) != 0)
        {
            std::string msg(sqlite3_errmsg(this->_sqliteDb));
            sqlite3_close(this->_sqliteDb);
            this->_sqliteDb = 0;
            throw std::runtime_error("Cannot open sqlite database (settings: '" + settings + "'): " + msg);
        }
        this->_beginTransaction = this->CreateQuery("BEGIN");
        this->_endTransaction = this->CreateQuery("COMMIT");
    }

    Connection::~Connection()
    {
        if (this->_sqliteDb != 0)
            sqlite3_close(this->_sqliteDb);
        this->_sqliteDb = 0;
    }

    std::unique_ptr<IQuery> Connection::CreateQuery(std::string const& request)
    {
        return std::unique_ptr<IQuery>(new Query(*this, request));
    }

    void Connection::BeginTransaction()
    {
        this->_beginTransaction->ExecuteNonSelect();
    }

    void Connection::EndTransaction()
    {
        this->_endTransaction->ExecuteNonSelect();
    }

}}}