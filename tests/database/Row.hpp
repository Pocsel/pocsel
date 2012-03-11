#ifndef __TOOLS_DATABASE_SQLITE_ROW_HPP__
#define __TOOLS_DATABASE_SQLITE_ROW_HPP__

//#include "tools/database/sqlite/Connection.hpp"
#include "Connection.hpp"
#include "Query.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Row : public IRow, private boost::noncopyable
    {
    private:
        Connection& _connection;
        sqlite3_stmt* _stmt;

    public:
        Row(Connection& connection, Query& query);

        virtual char GetChar(int colomn) const;
        virtual unsigned char GetUChar(int colomn) const;
        virtual short GetShort(int colomn) const;
        virtual unsigned short GetUShort(int colomn) const;
        virtual int GetInt(int colomn) const;
        virtual unsigned int GetUInt(int colomn) const;
        virtual long long GetLong(int colomn) const;
        virtual unsigned long long GetULong(int colomn) const;
        virtual float GetFloat(int colomn) const;
        virtual double GetDouble(int colomn) const;
        virtual std::string GetString(int colomn) const;
        virtual std::vector<char> GetArray(int colomn) const;
    };

}}}

#endif
