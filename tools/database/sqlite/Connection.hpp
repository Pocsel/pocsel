#ifndef __TOOLS_DATABASE_SQLITE_CONNECTION_HPP__
#define __TOOLS_DATABASE_SQLITE_CONNECTION_HPP__

#include <sqlite3.h>
#include "tools/database/IConnection.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Connection : public IConnection, private boost::noncopyable
    {
    private:
        sqlite3* _sqliteDb;
        bool _isInTransaction;

    public:
        Connection(std::string const& settings);
        virtual ~Connection();

        virtual std::unique_ptr<IQuery> CreateQuery(std::string const& request);
        virtual void BeginTransaction();
        virtual void EndTransaction();
        virtual bool HasTable(std::string const& table);
        virtual Int64 GetLastInsertedId();
        virtual bool IsInTransaction() const { return this->_isInTransaction; }
        sqlite3* GetSqliteHandler() { return this->_sqliteDb; }
    };

}}}

#endif
