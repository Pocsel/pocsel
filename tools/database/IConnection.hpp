#ifndef __TOOLS_DATABASE_ICONNECTION_HPP__
#define __TOOLS_DATABASE_ICONNECTION_HPP__

#include "tools/database/ICursor.hpp"

namespace Tools { namespace Database {

    class IConnection
    {
    public:
        virtual void Close() = 0;
        virtual ICursor& GetCursor() = 0;
        virtual void Commit() = 0;
        virtual bool HasTable(std::string const& table) = 0;
        virtual ~IConnection()
        {
        }
    };

}}

#endif
