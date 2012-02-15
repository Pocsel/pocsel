#include <boost/cstdlib.hpp>

#include "client2/Client.hpp"
#include "client2/Settings.hpp"
#include "client2/resources/LocalResourceManager.hpp"
#include "client2/window/sdl/Window.hpp"
#include "client2/window/InputManager.hpp"
#include "client2/network/Network.hpp"
#include "client2/network/PacketCreator.hpp"
#include "client2/network/PacketDispatcher.hpp"
#include "client2/menu/Menu.hpp"
#include "client2/menu/LoadingScreen.hpp"

#include "tools/Timer.hpp"

namespace Client {

    Client::Client(Settings& settings) :
        _settings(settings),
        _state(Connecting),
        _game(0)
    {
        this->_window = new Window::Sdl::Window(*this);
        this->_resourceManager = new Resources::LocalResourceManager(*this);
        this->_packetDispatcher = new Network::PacketDispatcher(*this);
        this->_menu = new Menu::Menu(*this);

        this->_window->GetInputManager().GetInputBinder().LoadFile(this->_settings.bindingsFile.string());
    }

    Client::~Client()
    {
        Tools::Delete(this->_game);
        Tools::Delete(this->_menu);
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
                this->_menu->GetLoadingScreen().Render("Connecting to " + this->_settings.host + ":" + this->_settings.port, 0);
                break;
            case LoadingResources:
                this->_menu->GetLoadingScreen().Render("Downloading resources", this->_game->GetLoadingProgression());
                if (this->_game->GetLoadingProgression() == 1.0f)
                {
                    Tools::debug << "LoadingChunks...\n";
                    this->_state = LoadingChunks;
                    this->_network.SendPacket(Network::PacketCreator::Settings(this->_settings));
                }
                break;
            case LoadingChunks:
                this->_menu->GetLoadingScreen().Render("Downloading chunks", this->_game->GetMap().GetLoadingProgression());
                break;
            case Running:
                this->_game->Update();
                this->_game->Render();
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
        this->_game = new Game::Game(*this, worldIdentifier, worldName, worldVersion, nbCubeTypes);
    }

    void Client::LoadChunks()
    {
        this->_state = LoadingChunks;
    }

    void Client::Disconnect(std::string const& reason)
    {
        this->_network.Stop();
        this->_state = Disconnected;
        delete this->_game;
    }

    void Client::Quit()
    {
        this->_state = Quitting;
    }
}
