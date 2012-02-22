#ifndef __CLIENT_CLIENT_HPP__
#define __CLIENT_CLIENT_HPP__

#include "common/BaseChunk.hpp"
#include "client/game/Game.hpp"
#include "client/network/Network.hpp"
#include "tools/thread/ThreadPool.hpp"

namespace Client {
    class Settings;
    namespace Game {
        class CubeTypeManager;
    }
    namespace Network {
        class PacketDispatcher;
    }
    namespace Resources {
        class LocalResourceManager;
    }
    namespace Window {
        class Window;
    }
    namespace Menu {
        class Menu;
    }
}

namespace Client {

    class Client :
        private boost::noncopyable
    {
    public:
        enum State
        {
            Quitting = 0,
            Connecting,
            LoggingIn,
            LoadingResources,
            WaitingPosition,
            LoadingChunks,
            Running,
            Disconnected,
        };

    private:
        Settings& _settings;
        Network::Network _network;
        Window::Window* _window;
        Resources::LocalResourceManager* _resourceManager;
        Network::PacketDispatcher* _packetDispatcher;
        State _state;
        Game::Game* _game;
        Menu::Menu* _menu;
        Tools::Thread::ThreadPool _threadPool;

    public:
        Client(Settings& settings);
        ~Client();
        int Run();
        void Login(std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes);
        void LoadChunks();
        void Connect();
        void Disconnect(std::string const& reason = "");
        void Quit();

        Settings& GetSettings() { return this->_settings; }
        Network::Network& GetNetwork() { return this->_network; }
        Window::Window& GetWindow() { return *this->_window; }
        Resources::LocalResourceManager& GetLocalResourceManager() { return *this->_resourceManager; }
        Game::Game& GetGame() { return *this->_game; }
        Menu::Menu& GetMenu() { return *this->_menu; }
        State GetState() const { return this->_state; }
        Tools::Thread::ThreadPool& GetThreadPool() { return this->_threadPool; }
    private:
        void _MenuBind();
    };

}

#endif
