#ifndef __TOOLS_DATABASE_SQLITE_CONNECTION_HPP__
#define __TOOLS_DATABASE_SQLITE_CONNECTION_HPP__

#include <sqlite3.h>
//#include "tools/database/IConnection.hpp"
#include "IConnection.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Connection : public IConnection, private boost::noncopyable
    {
    private:
        sqlite3* _sqliteDb;
        std::unique_ptr<IQuery> _beginTransaction;
        std::unique_ptr<IQuery> _endTransaction;

    public:
        Connection(std::string const& settings);
        virtual ~Connection();

        virtual std::unique_ptr<IQuery> CreateQuery(std::string const& request);
        virtual void BeginTransaction();
        virtual void EndTransaction();
        sqlite3* GetSqliteHandler() { return this->_sqliteDb; }
    };

}}}

#endif
