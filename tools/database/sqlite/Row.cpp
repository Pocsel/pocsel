#include "tools/precompiled.hpp"

#include "tools/database/sqlite/Row.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    Row::Row(Connection& connection, Query& query)
        : _connection(connection),
        _stmt(query.GetSqliteHandler())
    {
    }

    Int8 Row::GetInt8(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    Uint8 Row::GetUint8(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    Int16 Row::GetInt16(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    Uint16 Row::GetUint16(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    Int32 Row::GetInt32(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    Uint32 Row::GetUint32(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    Int64 Row::GetInt64(int colomn) const
    {
        return sqlite3_column_int64(this->_stmt, colomn);
    }

    Uint64 Row::GetUint64(int colomn) const
    {
        return sqlite3_column_int64(this->_stmt, colomn);
    }

    float Row::GetFloat(int colomn) const
    {
        return (float)sqlite3_column_double(this->_stmt, colomn);
    }

    double Row::GetDouble(int colomn) const
    {
        return sqlite3_column_double(this->_stmt, colomn);
    }

    std::string Row::GetString(int colomn) const
    {
        return std::string((char*)sqlite3_column_text(this->_stmt, colomn), sqlite3_column_bytes(this->_stmt, colomn));
    }

    std::vector<char> Row::GetArray(int colomn) const
    {
        auto ptr = sqlite3_column_blob(this->_stmt, colomn);
        auto size = sqlite3_column_bytes(this->_stmt, colomn);
        std::vector<char> data(size);
        std::memcpy(data.data(), ptr, size);
        return data;
    }

}}}
