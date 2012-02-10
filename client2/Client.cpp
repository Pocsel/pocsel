#include <boost/cstdlib.hpp>

#include "client2/Client.hpp"
#include "client2/game/Game.hpp"
#include "client2/window/sdl/Window.hpp"
#include "client2/window/InputManager.hpp"
#include "client2/network/Network.hpp"
#include "client2/network/PacketCreator.hpp"
#include "client2/network/PacketDispatcher.hpp"

#include "tools/Timer.hpp"

namespace Client {

    Client::Client(int ac, char** av)
        : _settings(ac, av),
        _state(Connecting),
        _game(0)
    {
        this->_window = new Window::Sdl::Window(*this);
        this->_packetDispatcher = new Network::PacketDispatcher(*this);

        this->_window->GetInputManager().GetInputBinder().Bind("eScApE", "quiT");
        this->_window->GetInputManager().Bind(BindAction::Quit, BindAction::Released, std::bind(&Client::Quit, this));
    }

    Client::~Client()
    {
        Tools::Delete(this->_game);
        Tools::Delete(this->_packetDispatcher);
        Tools::Delete(this->_window);
    }

    int Client::Run()
    {
        Tools::Timer frameTimer;
        this->_network.Connect(this->_settings.host, this->_settings.port);
        this->_network.SendPacket(Network::PacketCreator::Login("yalap_a"));
        while (this->_state)
        {
            frameTimer.Reset();

            if (this->_network.IsConnected())
                this->_packetDispatcher->ProcessAllPackets(this->_network.ProcessInPackets());
            else if (this->_state != Client::Disconnected)
                this->Disconnect("Unknown reason");
            this->_window->GetInputManager().ProcessEvents();
            this->_window->GetInputManager().DispatchActions();

            switch (this->_state)
            {
            case Connecting:
                break;
            case LoadingResources:
                if (this->_game->GetLoadingProgression() == 1.0f)
                {
                    Tools::debug << "LoadingChunks...\n";
                    this->_state = LoadingChunks;
                    // TODO:
                    //this->_network.SendPacket(Network::PacketCreator::Settings());
                }
                break;
            case LoadingChunks:
                break;
            case Running:
                break;
            case Disconnected:
                break;
            default:
                ;
            }

            this->_window->Render();

            int timeLeft = 1000 / this->_settings.fps - frameTimer.GetElapsedTime();
            if (timeLeft > 2)
                frameTimer.Sleep(timeLeft);
        }
        this->_network.Stop();
        return boost::exit_success;
    }

    void Client::Login(std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes)
    {
        if (this->_state != Connecting)
            throw std::runtime_error("Bad client state");
        this->_state = LoadingResources;
        this->_game = new Game::Game(*this, this->_network.GetHost(), worldIdentifier, worldName, worldVersion, nbCubeTypes);
    }

    void Client::Disconnect(std::string const& reason)
    {
        this->_network.Stop();
        this->_state = Disconnected;
    }

    void Client::Quit()
    {
        this->_state = Quitting;
    }
}
