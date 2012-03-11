#ifndef __TOOLS_DATABASE_SQLITE_STATEMENT_HPP__
#define __TOOLS_DATABASE_SQLITE_STATEMENT_HPP__

#include "tools/database/DatabaseError.hpp"
#include "tools/database/IStatement.hpp"
#include "tools/database/status.hpp"

namespace Tools { namespace Database { namespace Sqlite {

    class Statement : public Tools::Database::IStatement
    {
    private:
        ::sqlite3* _db;
        ::sqlite3_stmt* _stmt;
        int _index;

    public:
        Statement(::sqlite3* db, ::sqlite3_stmt* stmt);
        virtual ~Statement();
        virtual Tools::Database::IStatement& Bind(int val);
        virtual Tools::Database::IStatement& Bind(unsigned int val);
        virtual Tools::Database::IStatement& Bind(Int64 val);
        virtual Tools::Database::IStatement& Bind(Uint64 val);
        virtual Tools::Database::IStatement& Bind(float val);
        virtual Tools::Database::IStatement& Bind(double val);
        virtual Tools::Database::IStatement& Bind(bool val);
        virtual Tools::Database::IStatement& Bind(std::string const& s);
        virtual Tools::Database::IStatement& Bind(const char* s);
        virtual Tools::Database::IStatement& Bind(Tools::Database::Blob& val);
        virtual void Reset();
        Tools::Database::Status::Type Step();
        ::sqlite3_stmt* getSqlite3Stmt() { return this->_stmt; }

    private:
        template<typename T> void _raiseError(T val);
    };

}}}

#endif
