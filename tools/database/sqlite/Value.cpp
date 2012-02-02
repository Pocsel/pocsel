
#include "Value.hpp"

using namespace Tools::Database::Sqlite;

Value::Value(::sqlite3* db, ::sqlite3_stmt* stmt, unsigned int index) :
    _db(db), _stmt(stmt), _index(index)
{
}

int Value::GetInt()
{
    int res = ::sqlite3_column_int(this->_stmt, this->_index);
    if (res == 0 && ::sqlite3_errcode(this->_db) == SQLITE_NOMEM)
        throw std::runtime_error("Out of memory");
    return res;
}

Int64 Value::GetInt64()
{
    Int64 res = ::sqlite3_column_int64(this->_stmt, this->_index);
    if (res == 0 && ::sqlite3_errcode(this->_db) == SQLITE_NOMEM)
        throw std::runtime_error("Out of memory");
    return res;
}


Uint64 Value::GetUint64()
{
    Uint64 res = ::sqlite3_column_int64(this->_stmt, this->_index);
    if (res == 0 && ::sqlite3_errcode(this->_db) == SQLITE_NOMEM)
        throw std::runtime_error("Out of memory");
    return res;
}

float Value::GetFloat()
{
    float res = static_cast<float>(::sqlite3_column_double(this->_stmt, this->_index));
    if (res == 0 && ::sqlite3_errcode(this->_db) == SQLITE_NOMEM)
        throw std::runtime_error("Out of memory");
    return res;
}

double Value::GetDouble()
{
    double res = ::sqlite3_column_double(this->_stmt, this->_index);
    if (res == 0 && ::sqlite3_errcode(this->_db) == SQLITE_NOMEM)
        throw std::runtime_error("Out of memory");
    return res;
}

std::string Value::GetString()
{
    const unsigned char* res = ::sqlite3_column_text(this->_stmt, this->_index);
    if (res == 0 && ::sqlite3_errcode(this->_db) == SQLITE_NOMEM)
        throw std::runtime_error("Out of memory");
    return std::string((const char*) res);
}

bool Value::GetBool()
{
    int res = ::sqlite3_column_int(this->_stmt, this->_index);
    if (res == 0 && ::sqlite3_errcode(this->_db) == SQLITE_NOMEM)
        throw std::runtime_error("Out of memory");
    return res != 0;
}

Tools::Database::Blob Value::GetBlob()
{
    void const* res = ::sqlite3_column_blob(this->_stmt, this->_index);
    if (res == 0 && ::sqlite3_errcode(this->_db) == SQLITE_NOMEM)
        throw std::runtime_error("Out of memory");
    return Blob(res, sqlite3_column_bytes(this->_stmt, this->_index));
}


