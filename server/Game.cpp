#include <iostream>
#include <boost/thread.hpp>

#include "tools/database/ConnectionPool.hpp"
#include "tools/database/sqlite/Connection.hpp"
#include "tools/filesystem.hpp"

#include "server/ClientManager.hpp"
#include "server/EventManager.hpp"
#include "server/Game.hpp"
#include "server/Settings.hpp"
#include "server/World.hpp"

namespace Server {

    Game::Game(Settings const& settings, boost::asio::io_service& io) :
        _ioService(io),
        _eventManager(),
        _clientManager(*this),
        _connectionPool(0),
        _resourceManager(0),
        _world(0),
        _isRunning(false)

    {
        if (!boost::filesystem::exists(settings.worldFile))
            throw std::runtime_error("Cannot find map file " + settings.worldFile);

        using namespace Tools::Database;
        this->_connectionPool = new ConnectionPool<Sqlite::Connection>(settings.worldFile);
        this->_resourceManager = new ResourceManager(*this->_connectionPool);

        this->_world = new World(*this->_connectionPool);
    }

    Game::~Game()
    {
        Tools::Delete(this->_world);
        Tools::Delete(this->_connectionPool);
        Tools::Delete(this->_resourceManager);
    }

    ResourceManager& Game::GetResourceManager()
    {
        assert(this->_resourceManager != 0);
        return *this->_resourceManager;
    }

    int Game::Run()
    {
        assert(this->_isRunning == false);
        std::cout << "Starting game" << std::endl;
        assert(this->_world != 0);
        this->_isRunning = true;

        // EventManager
        boost::thread_group tg;
        for (int i = 0; i < 4; ++i)
            tg.create_thread(boost::bind(&EventManager::Run, &this->_eventManager));

        this->_ioService.run();

        tg.join_all();
        return 0;
    }

    void Game::Stop()
    {
        assert(this->_isRunning == true);
        std::cout << "Stopping game" << std::endl;
        this->_clientManager.Stop();
        this->_eventManager.Stop();
        this->_isRunning = false;
    }

    World& Game::GetWorld()
    {
        assert(this->_world != 0);
        return *this->_world;
    }

}
