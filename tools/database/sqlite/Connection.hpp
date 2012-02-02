#ifndef __TOOLS_DATABASE_SQLITE_CONNECTION_HPP__
#define __TOOLS_DATABASE_SQLITE_CONNECTION_HPP__

#include <list>
#include <cassert>
#include <sqlite3.h>

#include "tools/database/IConnection.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Connection : public Tools::Database::IConnection
    {
    private:
        ::sqlite3* _db;
        std::list< Tools::Database::ICursor* > _cursors;

    public:
        Connection(const char* filename);
        Connection(Connection const& conn);
        Connection& operator =(Connection const& conn);
        virtual ~Connection();
        virtual void Close();
        virtual Tools::Database::ICursor& GetCursor();
        virtual void Commit();
        virtual bool HasTable(std::string const& table)
        {
            auto& curs = this->GetCursor();
            curs.Execute("SELECT 1 FROM sqlite_master WHERE name = ?").Bind(table);
            return curs.HasData();
        }
    };

    static inline Tools::Database::IConnection* Open(const char* filename)
    {
        return new Connection(filename);
    }

    static inline void Close(Tools::Database::IConnection* conn)
    {
        assert(dynamic_cast<Connection*>(conn) != 0 && "Wrong connection type given !");
        delete conn;
    }

}}}

#endif
