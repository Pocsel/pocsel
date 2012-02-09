#ifndef __SERVER_SERVER_HPP__
#define __SERVER_SERVER_HPP__

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
        Settings* _settings;
        Network::Network* _network;
        Database::ResourceManager* _resourceManager;
        ClientManagement::ClientManager* _clientManager;
        Game::Game* _game;

    public:
        Server(int ac, char *av[]);
        ~Server();
        int Run();
        void Stop();
        Settings const& GetSettings() const { return *this->_settings; }
        Database::ResourceManager& GetResourceManager() { return *this->_resourceManager; }
        ClientManagement::ClientManager& GetClientManager() { return *this->_clientManager; }
        Game::Game& GetGame() { return *this->_game; }
    };

}

#endif
