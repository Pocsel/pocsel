#ifndef __TOOLS_DATABASE_SQLITE_ROW_HPP__
#define __TOOLS_DATABASE_SQLITE_ROW_HPP__

#include "tools/database/sqlite/Connection.hpp"
#include "tools/database/sqlite/Query.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Row : public IRow, private boost::noncopyable
    {
    private:
        Connection& _connection;
        sqlite3_stmt* _stmt;

    public:
        Row(Connection& connection, Query& query);

        virtual char GetChar(int colomn) const;
        virtual unsigned char GetUchar(int colomn) const;
        virtual short GetInt16(int colomn) const;
        virtual unsigned short GetUint16(int colomn) const;
        virtual int GetInt(int colomn) const;
        virtual unsigned int GetUint(int colomn) const;
        virtual long long GetInt64(int colomn) const;
        virtual unsigned long long GetUint64(int colomn) const;
        virtual float GetFloat(int colomn) const;
        virtual double GetDouble(int colomn) const;
        virtual std::string GetString(int colomn) const;
        virtual std::vector<char> GetArray(int colomn) const;
    };

}}}

#endif
