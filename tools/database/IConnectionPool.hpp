#ifndef __TOOLS_DATABASE_ICONNECTIONPOOL_HPP__
#define __TOOLS_DATABASE_ICONNECTIONPOOL_HPP__

#include <memory>

#include "tools/database/IConnection.hpp"

namespace Tools { namespace Database {

    class IConnectionPool
    {
    public:
        typedef std::shared_ptr<IConnection> ConnectionPtr;

    public:
        virtual std::shared_ptr<IConnection> GetConnection() = 0;
        virtual ~IConnectionPool() {}
    };

}}

#endif
