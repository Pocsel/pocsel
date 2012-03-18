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
        Uint32 _nbColumn;

    public:
        Row(Connection& connection, Query& query);

        virtual Uint32 GetColumnCount() const { return this->_nbColumn; }
        virtual Int8 GetInt8(Uint32 column) const;
        virtual Uint8 GetUint8(Uint32 column) const;
        virtual Int16 GetInt16(Uint32 column) const;
        virtual Uint16 GetUint16(Uint32 column) const;
        virtual Int32 GetInt32(Uint32 column) const;
        virtual Uint32 GetUint32(Uint32 column) const;
        virtual Int64 GetInt64(Uint32 column) const;
        virtual Uint64 GetUint64(Uint32 column) const;
        virtual float GetFloat(Uint32 column) const;
        virtual double GetDouble(Uint32 column) const;
        virtual std::string GetString(Uint32 column) const;
        virtual std::vector<char> GetArray(Uint32 column) const;
    };

}}}

#endif
