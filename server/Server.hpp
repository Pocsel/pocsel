#ifndef __SERVER_SERVER_HPP__
#define __SERVER_SERVER_HPP__

namespace Tools {

    class SimpleMessageQueue;

}

namespace Server {
    namespace Network {
        class Network;
    }
    namespace Database {
        class ResourceManager;
    }
    namespace ClientManagement {
        class ClientManager;
    }
    namespace Game {
        class Game;
    }
}

namespace Server {

    class Settings;

    class Server :
        private boost::noncopyable
    {
    private:
        Settings& _settings;
        Network::Network* _network;
        Database::ResourceManager* _resourceManager;
        ClientManagement::ClientManager* _clientManager;
        Game::Game* _game;
        Tools::SimpleMessageQueue* _admMessageQueue;

    public:
        Server(Settings& settings);
        ~Server();
        int Run();
        void Stop();
        Settings const& GetSettings() const { return this->_settings; }
        Database::ResourceManager const& GetResourceManager() const { return *this->_resourceManager; }
        Database::ResourceManager& GetResourceManager() { return *this->_resourceManager; }
        ClientManagement::ClientManager const& GetClientManager() const { return *this->_clientManager; }
        ClientManagement::ClientManager& GetClientManager() { return *this->_clientManager; }
        Game::Game const& GetGame() const { return *this->_game; }
        Game::Game& GetGame() { return *this->_game; }
    };

}

#endif
