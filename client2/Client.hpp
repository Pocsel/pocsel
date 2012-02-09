#ifndef __CLIENT_CLIENT_HPP__
#define __CLIENT_CLIENT_HPP__

#include <boost/noncopyable.hpp>

namespace Client {
    namespace Network {
        class Network;
    }
    namespace Window {
        class IWindow;
    }
}

namespace Client {

    class Settings;

    class Client :
        private boost::noncopyable
    {
    private:
        Settings* _settings;
        Window::IWindow* _window;
        Network::Network* _network;
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
