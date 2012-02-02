
#include <sqlite3.h>

#include "tools/database/DatabaseError.hpp"
#include "Connection.hpp"
#include "Cursor.hpp"

using namespace Tools::Database::Sqlite;

Connection::Connection(const char* filename) : _db(0), _cursors()
{
    int res = ::sqlite3_open(filename, &this->_db);
    if (res != 0)
    {
        std::string msg(::sqlite3_errmsg(this->_db));
        ::sqlite3_close(this->_db);
        this->_db = 0;
        throw Tools::Database::DatabaseError(
            "Cannot open '" + std::string(filename) + "': " + msg
        );
    }
}

Connection::Connection(Connection const& conn) : _db(conn._db), _cursors() {}

Connection& Connection::operator =(Connection const& conn)
{
    if (this != &conn)
    {
        this->_db = conn._db;
        // We do not copy cursors, since life of first connection object matters
    }
    return *this;
}

Connection::~Connection()
{
    this->Close();
}

void Connection::Close()
{
    std::list<Tools::Database::ICursor*>::iterator it = this->_cursors.begin(),
                                      end = this->_cursors.end();
    for (; it != end; ++it)
        delete (*it);
    this->_cursors.clear();
    if (this->_db != 0)
        ::sqlite3_close(this->_db);
    this->_db = 0;
}

::Tools::Database::ICursor& Connection::GetCursor()
{
    ::Tools::Database::ICursor* curs = new Cursor(this->_db);
    this->_cursors.push_back(curs);
    return *curs;
}

void Connection::Commit()
{

}

