#ifndef __TOOLS_DATABASE_SQLITE_QUERY_HPP__
#define __TOOLS_DATABASE_SQLITE_QUERY_HPP__

//#include "tools/database/sqlite/Connection.hpp"
#include "Connection.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Query : public IQuery, private boost::noncopyable
    {
    private:
        Connection& _connection;
        sqlite3_stmt* _stmt;
        int _bindIdx;

    public:
        Query(Connection& connection, std::string const& request);
        virtual ~Query();

        virtual IQuery& Bind(char val);
        virtual IQuery& Bind(unsigned char val);
        virtual IQuery& Bind(short val);
        virtual IQuery& Bind(unsigned short val);
        virtual IQuery& Bind(int val);
        virtual IQuery& Bind(unsigned int val);
        virtual IQuery& Bind(long long val);
        virtual IQuery& Bind(unsigned long long val);
        virtual IQuery& Bind(float val);
        virtual IQuery& Bind(double val);
        virtual IQuery& Bind(std::string const& val);
        virtual IQuery& Bind(void const* tab, std::size_t size);
        virtual IQuery& ExecuteNonSelect();
        virtual std::unique_ptr<IRow> Fetch();
        virtual void Reset();
        sqlite3_stmt* GetSqliteHandler() { return this->_stmt; }
    };

}}}

#endif
