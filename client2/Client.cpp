#include <boost/cstdlib.hpp>

#include "client2/Client.hpp"
#include "client2/window/sdl/Window.hpp"
#include "client2/window/IInputManager.hpp"
#include "client2/network/Network.hpp"
#include "client2/Settings.hpp"

#include "tools/Timer.hpp"

namespace Client {

    Client::Client(int ac, char** av) :
        _running(true)
    {
        this->_settings = new Settings(ac, av);
        this->_window = new Window::Sdl::Window(*this);
        this->_network = new Network::Network(*this);
    }

    Client::~Client()
    {
        Tools::Delete(this->_network);
        Tools::Delete(this->_window);
        Tools::Delete(this->_settings);
    }

    int Client::Run()
    {
        boost::thread networkThread(std::bind(&Network::Network::Run, this->_network));
        Tools::Timer frameTimer;
        while (this->_running)
        {
            frameTimer.Reset();

            this->_window->GetInputManager().ProcessEvents();
            this->_window->Render();

            int timeLeft = 1000 / this->_settings->fps - frameTimer.GetElapsedTime();
            if (timeLeft > 2)
                frameTimer.Sleep(timeLeft);
        }
        this->_network->Stop();
        networkThread.join();
        return boost::exit_success;
    }

    Network::Network& Client::GetNetwork()
    {
        return *this->_network;
    }

    Settings& Client::GetSettings()
    {
        return *this->_settings;
    }

    Window::IWindow& Client::GetWindow()
    {
        return *this->_window;
    }

    void Client::Quit()
    {
        this->_running = false;
    }
}
