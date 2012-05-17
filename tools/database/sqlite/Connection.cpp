#include "tools/precompiled.hpp"

#include <sqlite3.h>
#include "tools/database/sqlite/Connection.hpp"
#include "tools/database/sqlite/Query.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    Connection::Connection(std::string const& settings)
    {
        if (sqlite3_open(settings.c_str(), &this->_sqliteDb) != SQLITE_OK)
        {
            std::string msg(sqlite3_errmsg(this->_sqliteDb));
            sqlite3_close(this->_sqliteDb);
            this->_sqliteDb = 0;
            throw std::runtime_error("Cannot open sqlite database (settings: '" + settings + "'): " + msg);
        }
        this->_beginTransaction = this->CreateQuery("BEGIN").release();
        this->_endTransaction = this->CreateQuery("COMMIT").release();
    }

    Connection::~Connection()
    {
        Tools::Delete(this->_beginTransaction);
        Tools::Delete(this->_endTransaction);
        if (this->_sqliteDb != 0)
            if (sqlite3_close(this->_sqliteDb) != SQLITE_OK)
                throw std::runtime_error("Cannot close sqlite database");
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

    bool Connection::HasTable(std::string const& table)
    {
        auto query = this->CreateQuery("SELECT 1 FROM sqlite_master WHERE name = ?");
        query->Bind(table);
        return query->Fetch().get() != 0;
    }

    Int64 Connection::GetLastInsertedId()
    {
        return sqlite3_last_insert_rowid(this->_sqliteDb);
    }

}}}
