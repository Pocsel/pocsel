#ifndef __SERVER_GAME_HPP__
# define __SERVER_GAME_HPP__


# include <memory>
# include <boost/noncopyable.hpp>
# include <boost/asio/io_service.hpp>

# include "tools/database/IConnectionPool.hpp"

# include "server/ClientManager.hpp"
# include "server/EventManager.hpp"

namespace Server {

    class World;
    class Map;
    class ResourceManager;
    class Settings;

    class Game : private boost::noncopyable
    {
    private:
        boost::asio::io_service&            _ioService; // Network io_service
        EventManager                        _eventManager;
        ClientManager                       _clientManager;
        Tools::Database::IConnectionPool*   _connectionPool;
        ResourceManager*                    _resourceManager;
        World*                              _world;
        volatile bool                       _isRunning;

    public:
        Game(Settings const& settings, boost::asio::io_service& io);
        int Run();
        void Stop();
        ~Game();

        inline ClientManager& GetClientManager() { return this->_clientManager; }
        inline EventManager& GetEventManager() { return this->_eventManager; }
        ResourceManager& GetResourceManager();
        World& GetWorld();

    };

}

#endif
