#include <boost/cstdlib.hpp>

#include "tools/Timer.hpp"
#include "tools/stat/StatManager.hpp"
#include "tools/window/sdl/Window.hpp"
#include "tools/window/InputManager.hpp"
#include "tools/sound/fmod/SoundSystem.hpp"

#include "client/Client.hpp"
#include "client/Settings.hpp"
#include "client/resources/LocalResourceManager.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"
#include "client/network/PacketDispatcher.hpp"
#include "client/map/Map.hpp"
#include "client/menu/Menu.hpp"
#include "client/menu/LoadingScreen.hpp"
#include "client/menu/DisconnectedScreen.hpp"
#include "client/menu/MainMenu.hpp"

namespace Client {

    Client::Client(Settings& settings) :
        _settings(settings),
        _state(Connecting),
        _game(0)
    {
        std::map<std::string, Tools::Window::BindAction::BindAction> actions;
        actions["quit"] = Tools::Window::BindAction::Quit;
        actions["menu"] = Tools::Window::BindAction::Menu;
        actions["forward"] = Tools::Window::BindAction::Forward;
        actions["backward"] = Tools::Window::BindAction::Backward;
        actions["left"] = Tools::Window::BindAction::Left;
        actions["right"] = Tools::Window::BindAction::Right;
        actions["jump"] = Tools::Window::BindAction::Jump;
        actions["crouch"] = Tools::Window::BindAction::Crouch;
        actions["fire"] = Tools::Window::BindAction::Fire;
        actions["altfire"] = Tools::Window::BindAction::AltFire;
        actions["use"] = Tools::Window::BindAction::Use;
        actions["togglesprint"] = Tools::Window::BindAction::ToggleSprint;
        actions["toggle-draw-physics"] = Tools::Window::BindAction::ToggleDrawPhysics;

        this->_window = new Tools::Window::Sdl::Window(actions, this->_settings.useDirect3D9, this->_settings.res, this->_settings.fullscreen);
        this->_effectManager = new Tools::Gfx::Effect::EffectManager(this->_window->GetRenderer());
        this->_threadPool = new Tools::Thread::ThreadPool(2);
        this->_soundSystem = new Tools::Sound::Fmod::SoundSystem();
        this->_resourceManager = new Resources::LocalResourceManager(*this);
        this->_packetDispatcher = new Network::PacketDispatcher(*this);
        this->_menu = new Menu::Menu(*this);

        this->_window->GetInputManager().Bind(Tools::Window::BindAction::Quit, Tools::Window::BindAction::Released, std::bind(&Client::Quit, this));
        this->_window->GetInputManager().Bind(Tools::Window::BindAction::Menu, Tools::Window::BindAction::Released, std::bind(&Client::_MenuBind, this));
        this->_window->GetInputManager().Bind(Tools::Window::BindAction::ToggleDrawPhysics, Tools::Window::BindAction::Released, std::bind(&Client::_ToggleDrawPhysicsBind, this));
        this->_window->GetInputManager().GetInputBinder().LoadFile(this->_settings.bindingsFile.string());
    }

    Client::~Client()
    {
        // destroy game first
        Tools::Delete(this->_game);
        // game resources
        Tools::Delete(this->_packetDispatcher);
        Tools::Delete(this->_threadPool);
        // client resources
        Tools::Delete(this->_menu);
        Tools::Delete(this->_resourceManager);
        // low level systems (sound & gfx)
        Tools::Delete(this->_soundSystem);
        Tools::Delete(this->_effectManager);
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
                if (!this->_window->GetInputManager().IsMinimized())
                    this->_menu->GetLoadingScreen().Render("Connecting to " + this->_settings.host + ":" + this->_settings.port + "...",
                        this->_network.GetLoadingProgression());
                if (this->_network.IsConnected())
                {
                    this->_network.SendPacket(Network::PacketCreator::Login("yalap_a"));
                    this->_state = LoggingIn;
                }
                break;
            case LoggingIn:
                if (!this->_window->GetInputManager().IsMinimized())
                    this->_menu->GetLoadingScreen().Render("Logging in...", this->_network.GetLoadingProgression());
                break;
            case LoadingResources:
                if (!this->_window->GetInputManager().IsMinimized())
                    this->_menu->GetLoadingScreen().Render("Downloading resources...", this->_game->GetLoadingProgression());
                if (this->_game->GetLoadingProgression() >= 1.0f)
                {
                    this->_state = WaitingPosition;
                    this->_network.SendPacket(Network::PacketCreator::Settings(this->_settings));
                    this->_game->LoadResources();
                }
                break;
            case WaitingPosition:
                if (!this->_window->GetInputManager().IsMinimized())
                    this->_menu->GetLoadingScreen().Render("Waiting for position...", 0);
                break;
            case LoadingChunks:
                if (!this->_window->GetInputManager().IsMinimized())
                    this->_menu->GetLoadingScreen().Render("Downloading & generating chunks...", this->_game->GetMap().GetLoadingProgression());
                this->_game->GetMap().GetChunkManager().UpdateLoading();
                if (this->_game->GetMap().GetLoadingProgression() >= 1.0f)
                {
                    Tools::debug << "Run!\n";
                    this->_state = Running;
                    this->_network.SendPacket(Network::PacketCreator::TeleportOk());
                }
                break;
            case Running:
                this->_game->Update();
                if (!this->_window->GetInputManager().IsMinimized())
                    this->_game->Render();
                break;
            case Disconnected:
                if (!this->_window->GetInputManager().IsMinimized())
                    this->_menu->GetDisconnectedScreen().Render();
                break;
            default:
                ;
            }

            if (this->_menu->GetMainMenu().IsVisible())
                this->_menu->GetMainMenu().Render();
            else if (!this->_window->GetInputManager().HasFocus() || this->_window->GetInputManager().IsMinimized())
                this->_menu->GetMainMenu().SetVisible(true /* visible */, false /* warpMouse */);

            // dispatch actions that were not dispatched
            this->_window->GetInputManager().Dispatch(this->_window->GetInputManager(), true);

            if (!this->_window->GetInputManager().IsMinimized())
                this->_window->Render();

            int timeLeft = 1000 / this->_settings.fps - frameTimer.GetElapsedTime();
            if (timeLeft > 2)
                frameTimer.Sleep(timeLeft);

            Tools::Stat::statManager.Update();
            this->_soundSystem->Update();
        }

        if (this->_network.IsRunning())
            this->_network.Stop();

        return boost::exit_success;
    }

    void Client::Login(Uint32 clientId,
            std::string const& worldIdentifier,
            std::string const& worldName,
            Uint32 worldVersion,
            Common::BaseChunk::CubeType nbCubeTypes,
            Uint32 nbBodyTypes,
            std::string const& worldBuildHash)
    {
        if (this->_state != LoggingIn)
            throw std::runtime_error("Bad client state");
        this->_clientId = clientId;
        this->_network.SetId(clientId);
        this->_state = LoadingResources;
        if (this->_game)
            Tools::Delete(this->_game);
        this->_game = new Game::Game(*this, worldIdentifier, worldName, worldVersion, nbCubeTypes, nbBodyTypes, worldBuildHash);
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
        Tools::Delete(this->_game);
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

    void Client::_ToggleDrawPhysicsBind()
    {
        this->_settings.drawPhysics = !this->_settings.drawPhysics;
    }

}
