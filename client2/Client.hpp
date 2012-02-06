#ifndef __CLIENT_CLIENT_HPP__
#define __CLIENT_CLIENT_HPP__

#include <boost/noncopyable.hpp>

namespace Client {

    class Settings;
    class IWindow;
    class Network;

    class Client :
        private boost::noncopyable
    {
        private:
            Settings* _settings;
            IWindow* _window;
            Network* _network;
            bool _running;

        public:
            Client(int ac, char** av);
            ~Client();
            int Run();
            Settings& GetSettings();
            IWindow& GetWindow();
            Network& GetNetwork();
            void Quit();
    };

}

#endif
