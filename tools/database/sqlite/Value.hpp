#ifndef __TOOLS_DATABASE_SQLITE_VALUE_HPP__
#define __TOOLS_DATABASE_SQLITE_VALUE_HPP__

#include "tools/database/IValue.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Value : public Tools::Database::IValue
    {
    private:
        ::sqlite3* _db;
        ::sqlite3_stmt* _stmt;
        unsigned int _index;

    public:
        Value(::sqlite3* db, ::sqlite3_stmt* stmt, unsigned int index);
        virtual int GetInt();
        virtual Int32 GetInt32();
        virtual Uint32 GetUint32();
        virtual Int64 GetInt64();
        virtual Uint64 GetUint64();
        virtual float GetFloat();
        virtual double GetDouble();
        virtual std::string GetString();
        virtual bool GetBool();
        virtual Tools::Database::Blob GetBlob();

    private:
        Value(Value const& value);
        Value& operator =(Value const& value);
    };

}}}

#endif
