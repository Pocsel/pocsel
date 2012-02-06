#include <boost/cstdlib.hpp>

#include "client2/Client.hpp"
#include "client2/WindowSdl.hpp"
#include "client2/Network.hpp"
#include "client2/Settings.hpp"

#include "tools/Timer.hpp"

namespace Client {

    Client::Client(int ac, char** av) :
        _running(true)
    {
        this->_settings = new Settings(ac, av);
        this->_window = new WindowSdl(*this);
        this->_network = new Network(*this);
    }

    Client::~Client()
    {
        Tools::Delete(this->_network);
        Tools::Delete(this->_window);
        Tools::Delete(this->_settings);
    }

    int Client::Run()
    {
        boost::thread networkThread(std::bind(&Network::Run, this->_network));
        Tools::Timer frameTimer;
        while (this->_running)
        {
            frameTimer.Reset();

            this->_window->Render();

            int timeLeft = 1000 / this->_settings->fps - frameTimer.GetElapsedTime();
            if (timeLeft > 2)
                frameTimer.Sleep(timeLeft);
        }
        this->_network->Stop();
        networkThread.join();
        return boost::exit_success;
    }

    Settings& Client::GetSettings()
    {
        return *this->_settings;
    }

    IWindow& Client::GetWindow()
    {
        return *this->_window;
    }

    void Client::Quit()
    {
        this->_running = false;
    }
}
