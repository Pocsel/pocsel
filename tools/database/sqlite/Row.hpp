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

        virtual Int8 GetInt8(int column) const;
        virtual Uint8 GetUint8(int column) const;
        virtual Int16 GetInt16(int column) const;
        virtual Uint16 GetUint16(int column) const;
        virtual Int32 GetInt32(int column) const;
        virtual Uint32 GetUint32(int column) const;
        virtual Int64 GetInt64(int column) const;
        virtual Uint64 GetUint64(int column) const;
        virtual float GetFloat(int column) const;
        virtual double GetDouble(int column) const;
        virtual std::string GetString(int column) const;
        virtual std::vector<char> GetArray(int column) const;
    };

}}}

#endif
