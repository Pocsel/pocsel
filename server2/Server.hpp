#ifndef __SERVER_SERVER_HPP__
#define __SERVER_SERVER_HPP__

namespace Server {

    namespace Network {

        class Network;

    }

    namespace ClientManagement {

        class ClientManager;

    }

}

namespace Server {

    class Settings;
    //class Game;

    class Server :
        private boost::noncopyable
    {
    private:
        Settings* _settings;
        Network::Network* _network;
        ClientManagement::ClientManager* _clientManager;
        //Game* _game;

    public:
        Server(int ac, char *av[]);
        ~Server();
        int Run();
        void Stop();
        Settings const& GetSettings() const { return *this->_settings; }
        ClientManagement::ClientManager& GetClientManager() { return *this->_clientManager; }
    };

}

#endif
