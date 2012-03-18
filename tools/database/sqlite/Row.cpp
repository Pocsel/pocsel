#include "tools/precompiled.hpp"

#include "tools/database/sqlite/Row.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    Row::Row(Connection& connection, Query& query)
        : _connection(connection),
        _stmt(query.GetSqliteHandler())
    {
    }

    Int8 Row::GetInt8(int column) const
    {
        return sqlite3_column_int(this->_stmt, column);
    }

    Uint8 Row::GetUint8(int column) const
    {
        return sqlite3_column_int(this->_stmt, column);
    }

    Int16 Row::GetInt16(int column) const
    {
        return sqlite3_column_int(this->_stmt, column);
    }

    Uint16 Row::GetUint16(int column) const
    {
        return sqlite3_column_int(this->_stmt, column);
    }

    Int32 Row::GetInt32(int column) const
    {
        return sqlite3_column_int(this->_stmt, column);
    }

    Uint32 Row::GetUint32(int column) const
    {
        return sqlite3_column_int(this->_stmt, column);
    }

    Int64 Row::GetInt64(int column) const
    {
        return sqlite3_column_int64(this->_stmt, column);
    }

    Uint64 Row::GetUint64(int column) const
    {
        return sqlite3_column_int64(this->_stmt, column);
    }

    float Row::GetFloat(int column) const
    {
        return (float)sqlite3_column_double(this->_stmt, column);
    }

    double Row::GetDouble(int column) const
    {
        return sqlite3_column_double(this->_stmt, column);
    }

    std::string Row::GetString(int column) const
    {
        return std::string((char*)sqlite3_column_text(this->_stmt, column), sqlite3_column_bytes(this->_stmt, column));
    }

    std::vector<char> Row::GetArray(int column) const
    {
        auto ptr = sqlite3_column_blob(this->_stmt, column);
        auto size = sqlite3_column_bytes(this->_stmt, column);
        std::vector<char> data(size);
        std::memcpy(data.data(), ptr, size);
        return data;
    }

}}}
