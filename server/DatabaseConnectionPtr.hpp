#ifndef __SERVER_DATABASECONNECTIONPTR_HPP__
#define __SERVER_DATABASECONNECTIONPTR_HPP__

#include <memory>

#include "tools/database/IConnection.hpp"

namespace Server {

    typedef std::shared_ptr<Tools::Database::IConnection> DatabaseConnectionPtr;

}

#endif
