#include <boost/cstdlib.hpp>

#include "client/Client.hpp"
#include "client/Settings.hpp"
#include "client/resources/LocalResourceManager.hpp"
#include "client/window/sdl/Window.hpp"
#include "client/window/InputManager.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/network/PacketDispatcher.hpp"
#include "client/map/Map.hpp"
#include "client/menu/Menu.hpp"
#include "client/menu/LoadingScreen.hpp"
#include "client/menu/DisconnectedScreen.hpp"
#include "client/menu/MainMenu.hpp"

#include "tools/Timer.hpp"

namespace Client {

    Client::Client(Settings& settings) :
        _settings(settings),
        _state(Connecting),
        _game(0),
        _threadPool(2)
    {
        this->_window = new Window::Sdl::Window(true, this->_settings.res, this->_settings.fullscreen, this->_settings.useShaders);
        this->_resourceManager = new Resources::LocalResourceManager(*this);
        this->_packetDispatcher = new Network::PacketDispatcher(*this);
        this->_menu = new Menu::Menu(*this);

        this->_window->GetInputManager().Bind(BindAction::Quit, BindAction::Released, std::bind(&Client::Quit, this));
        this->_window->GetInputManager().Bind(BindAction::Menu, BindAction::Released, std::bind(&Client::_MenuBind, this));
        this->_window->GetInputManager().GetInputBinder().LoadFile(this->_settings.bindingsFile.string());
    }

    Client::~Client()
    {
        Tools::Delete(this->_game);
        Tools::Delete(this->_menu);
        Tools::Delete(this->_packetDispatcher);
        Tools::Delete(this->_resourceManager);
        Tools::Delete(this->_window);
    }

    int Client::Run()
    {
        Tools::Timer frameTimer;
        this->Connect();

        // main loop
        while (this->_state)
        {
            frameTimer.Reset();

            // process packets & check connection
            if (this->_network.IsConnected())
                this->_packetDispatcher->ProcessAllPackets(this->_network.GetInPackets());
            else if (this->_state != Client::Disconnected && !this->_network.GetLastError().empty())
            {
                this->_state = Client::Disconnected;
                this->_menu->GetDisconnectedScreen().SetMessage(this->_network.GetLastError());
            }

            // put events in buffer list
            this->_window->GetInputManager().ProcessEvents();

            if (this->_menu->GetMainMenu().IsVisible())
                this->_menu->GetMainMenu().Update();

            switch (this->_state)
            {
            case Connecting:
                this->_menu->GetLoadingScreen().Render("Connecting to " + this->_settings.host + ":" + this->_settings.port + "...",
                        this->_network.GetLoadingProgression());
                if (this->_network.IsConnected())
                {
                    this->_network.SendPacket(Network::PacketCreator::Login("yalap_a"));
                    this->_state = LoggingIn;
                }
                break;
            case LoggingIn:
                this->_menu->GetLoadingScreen().Render("Logging in...", this->_network.GetLoadingProgression());
                break;
            case LoadingResources:
                this->_menu->GetLoadingScreen().Render("Downloading resources...", this->_game->GetLoadingProgression());
                if (this->_game->GetLoadingProgression() >= 1.0f)
                {
                    this->_state = WaitingPosition;
                    this->_network.SendPacket(Network::PacketCreator::Settings(this->_settings));
                }
                break;
            case WaitingPosition:
                this->_menu->GetLoadingScreen().Render("Waiting for position...", 0);
                break;
            case LoadingChunks:
                this->_menu->GetLoadingScreen().Render("Downloading & generating chunks...", this->_game->GetMap().GetLoadingProgression());
                this->_game->GetMap().GetChunkManager().UpdateLoading();
                if (this->_game->GetMap().GetLoadingProgression() >= 1.0f)
                {
                    Tools::debug << "Run !\n";
                    this->_state = Running;
                    this->_network.SendPacket(Network::PacketCreator::TeleportOk());
                }
                break;
            case Running:
                this->_game->Update();
                this->_game->Render();
                break;
            case Disconnected:
                this->_menu->GetDisconnectedScreen().Render();
                break;
            default:
                ;
            }

            if (this->_menu->GetMainMenu().IsVisible())
                this->_menu->GetMainMenu().Render();

            // dispatch actions that were not dispatched
            this->_window->GetInputManager().Dispatch(this->_window->GetInputManager(), true);

            this->_window->Render();

            int timeLeft = 1000 / this->_settings.fps - frameTimer.GetElapsedTime();
            if (timeLeft > 2)
                frameTimer.Sleep(timeLeft);
        }

        if (this->_network.IsRunning())
            this->_network.Stop();

        return boost::exit_success;
    }

    void Client::Login(Uint32 clientId, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes, std::string const& worldBuildHash)
    {
        if (this->_state != LoggingIn)
            throw std::runtime_error("Bad client state");
        this->_clientId = clientId;
        this->_state = LoadingResources;
        if (this->_game)
            delete this->_game;
        this->_game = new Game::Game(*this, worldIdentifier, worldName, worldVersion, nbCubeTypes, worldBuildHash);
    }

    void Client::LoadChunks()
    {
        this->_state = LoadingChunks;
    }

    void Client::Connect()
    {
        if (this->_network.IsRunning())
            this->_network.Stop();
        this->_network.Connect(this->_settings.host, this->_settings.port);
        this->_state = Connecting;
    }

    void Client::Disconnect(std::string const& reason /* = "" */)
    {
        if (this->_network.IsRunning())
            this->_network.Stop();
        this->_menu->GetDisconnectedScreen().SetMessage(reason);
        delete this->_game;
        this->_game = 0;
        this->_state = Disconnected;
    }

    void Client::Quit()
    {
        if (this->_state == Connecting)
            exit(boost::exit_success); // ohoho c'est pas cool mais ça evite d'attendre le timeout dans io_service.stop()
        this->_state = Quitting;
    }

    void Client::_MenuBind()
    {
        this->_menu->GetMainMenu().SetVisible();
    }

}
