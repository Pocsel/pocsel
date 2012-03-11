#include "tools/precompiled.hpp"

#include "Row.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    Row::Row(Connection& connection, Query& query)
        : _connection(connection),
        _stmt(query.GetSqliteHandler())
    {
    }

    char Row::GetChar(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    unsigned char Row::GetUChar(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    short Row::GetShort(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    unsigned short Row::GetUShort(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    int Row::GetInt(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    unsigned int Row::GetUInt(int colomn) const
    {
        return sqlite3_column_int(this->_stmt, colomn);
    }

    long long Row::GetLong(int colomn) const
    {
        return sqlite3_column_int64(this->_stmt, colomn);
    }

    unsigned long long Row::GetULong(int colomn) const
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