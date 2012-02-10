#include <boost/cstdlib.hpp>

#include "client2/Client.hpp"
#include "client2/window/sdl/Window.hpp"
#include "client2/window/InputManager.hpp"
#include "client2/network/Network.hpp"
#include "client2/network/PacketCreator.hpp"
#include "client2/network/PacketDispatcher.hpp"

#include "tools/Timer.hpp"

namespace Client {

    Client::Client(int ac, char** av)
        : _settings(ac, av),
        _running(true)
    {
        this->_window = new Window::Sdl::Window(*this);
        this->_network = new Network::Network(*this);
        this->_packetDispatcher = new Network::PacketDispatcher(*this);
    }

    Client::~Client()
    {
        Tools::Delete(this->_packetDispatcher);
        Tools::Delete(this->_network);
        Tools::Delete(this->_window);
    }

    int Client::Run()
    {
        Tools::Timer frameTimer;
        this->_network->Connect(this->_settings.host, this->_settings.port);
        this->_network->SendPacket(Network::PacketCreator::Login("yalap_a"));
        while (this->_running)
        {
            frameTimer.Reset();

            this->_packetDispatcher->ProcessAllPackets(this->_network->ProcessInPackets());
            this->_window->GetInputManager().ProcessEvents();
            this->_window->GetInputManager().DispatchActions();
            this->_window->Render();

            int timeLeft = 1000 / this->_settings.fps - frameTimer.GetElapsedTime();
            if (timeLeft > 2)
                frameTimer.Sleep(timeLeft);
        }
        this->_network->Stop();
        return boost::exit_success;
    }

    Network::Network& Client::GetNetwork()
    {
        return *this->_network;
    }

    Settings& Client::GetSettings()
    {
        return this->_settings;
    }

    Window::Window& Client::GetWindow()
    {
        return *this->_window;
    }

    void Client::Quit()
    {
        this->_running = false;
    }
}
