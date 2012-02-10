#ifndef __CLIENT_CLIENT_HPP__
#define __CLIENT_CLIENT_HPP__

#include <boost/noncopyable.hpp>

#include "client2/Settings.hpp"

namespace Client {
    namespace Network {
        class Network;
        class PacketDispatcher;
    }
    namespace Window {
        class IWindow;
    }
}

namespace Client {

    class Client :
        private boost::noncopyable
    {
    private:
        Settings _settings;
        Window::IWindow* _window;
        Network::Network* _network;
        Network::PacketDispatcher* _packetDispatcher;
        bool _running;

    public:
        Client(int ac, char** av);
        ~Client();
        int Run();
        Settings& GetSettings();
        Window::IWindow& GetWindow();
        Network::Network& GetNetwork();
        void Quit();
    };

}

#endif
