#include "tools/precompiled.hpp"

#include "tools/database/sqlite/Row.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    Row::Row(Connection& connection, Query& query)
        : _connection(connection),
        _stmt(query.GetSqliteHandler())
    {
        this->_nbColumn = sqlite3_column_count(this->_stmt);
    }

    Int8 Row::GetInt8(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return sqlite3_column_int(this->_stmt, column);
    }

    Uint8 Row::GetUint8(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return sqlite3_column_int(this->_stmt, column);
    }

    Int16 Row::GetInt16(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return sqlite3_column_int(this->_stmt, column);
    }

    Uint16 Row::GetUint16(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return sqlite3_column_int(this->_stmt, column);
    }

    Int32 Row::GetInt32(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return sqlite3_column_int(this->_stmt, column);
    }

    Uint32 Row::GetUint32(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return sqlite3_column_int(this->_stmt, column);
    }

    Int64 Row::GetInt64(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return sqlite3_column_int64(this->_stmt, column);
    }

    Uint64 Row::GetUint64(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return sqlite3_column_int64(this->_stmt, column);
    }

    float Row::GetFloat(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return (float)sqlite3_column_double(this->_stmt, column);
    }

    double Row::GetDouble(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return sqlite3_column_double(this->_stmt, column);
    }

    std::string Row::GetString(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        return std::string((char*)sqlite3_column_text(this->_stmt, column), sqlite3_column_bytes(this->_stmt, column));
    }

    std::vector<char> Row::GetArray(Uint32 column) const
    {
        if (column >= this->_nbColumn)
            throw std::invalid_argument("Sqlite: Bad column id");
        auto ptr = sqlite3_column_blob(this->_stmt, column);
        auto size = sqlite3_column_bytes(this->_stmt, column);
        std::vector<char> data(size);
        std::memcpy(data.data(), ptr, size);
        return data;
    }

}}}
