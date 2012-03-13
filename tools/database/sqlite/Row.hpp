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

        virtual Int8 GetInt8(int colomn) const;
        virtual Uint8 GetUint8(int colomn) const;
        virtual Int16 GetInt16(int colomn) const;
        virtual Uint16 GetUint16(int colomn) const;
        virtual Int32 GetInt32(int colomn) const;
        virtual Uint32 GetUint32(int colomn) const;
        virtual Int64 GetInt64(int colomn) const;
        virtual Uint64 GetUint64(int colomn) const;
        virtual float GetFloat(int colomn) const;
        virtual double GetDouble(int colomn) const;
        virtual std::string GetString(int colomn) const;
        virtual std::vector<char> GetArray(int colomn) const;
    };

}}}

#endif
