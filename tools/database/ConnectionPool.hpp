#ifndef __TOOLS_DATABASE_CONNECTIONPOOL_HPP__
#define __TOOLS_DATABASE_CONNECTIONPOOL_HPP__

#include "IConnectionPool.hpp"

namespace Tools { namespace Database {

    template<typename ConnectionType>
    class ConnectionPool : public IConnectionPool
    {
    private:
        std::string _connectString;

    public:
        ConnectionPool(std::string const& str) :
            _connectString(str)
        {}

        virtual IConnectionPool::ConnectionPtr GetConnection()
        {
            auto ptr = new ConnectionType(this->_connectString.c_str());
            return IConnectionPool::ConnectionPtr(ptr);
        }
    };

}}

#endif
