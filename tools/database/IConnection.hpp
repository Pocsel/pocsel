#ifndef __TOOLS_DATABASE_ICONNECTION_HPP__
#define __TOOLS_DATABASE_ICONNECTION_HPP__

#include "tools/database/IQuery.hpp"

namespace Tools { namespace Database {

    class IConnection
    {
    public:
        virtual ~IConnection() {}
        virtual std::unique_ptr<IQuery> CreateQuery(std::string const& request) = 0;
        virtual void BeginTransaction() = 0;
        virtual void EndTransaction() = 0;
        virtual bool HasTable(std::string const& table) = 0;
        virtual Int64 GetLastInsertedId() = 0;
    };

}}

#endif
