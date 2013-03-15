#ifndef __CLIENT_CLIENT_HPP__
#define __CLIENT_CLIENT_HPP__

#include "common/BaseChunk.hpp"
#include "client/game/Game.hpp"
#include "client/network/Network.hpp"
#include "tools/thread/ThreadPool.hpp"

namespace Tools {
    namespace Gfx { namespace Effect {
        class EffectManager;
    }}
    namespace Window {
        class Window;
    }
}

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
        Tools::Window::Window* _window;
        Tools::Gfx::Effect::EffectManager* _effectManager;
        Resources::LocalResourceManager* _resourceManager;
        Network::PacketDispatcher* _packetDispatcher;
        State _state;
        Game::Game* _game;
        Menu::Menu* _menu;
        Tools::Thread::ThreadPool* _threadPool;
        Uint32 _clientId;

    public:
        Client(Settings& settings);
        ~Client();
        int Run();
        void Login(Uint32 clientId,
                std::string const& worldIdentifier,
                std::string const& worldName,
                Uint32 worldVersion,
                Common::BaseChunk::CubeType nbCubeTypes,
                Uint32 nbBodyTypes,
                std::string const& worldBuildHash);
        void LoadChunks();
        void Connect();
        void Disconnect(std::string const& reason = "");
        void Quit();

        Settings& GetSettings() { return this->_settings; }
        Network::Network& GetNetwork() { return this->_network; }
        Tools::Window::Window& GetWindow() { return *this->_window; }
        Tools::Gfx::Effect::EffectManager& GetEffectManager() { return *this->_effectManager; }
        Resources::LocalResourceManager& GetLocalResourceManager() { return *this->_resourceManager; }
        Game::Game& GetGame() { return *this->_game; }
        Menu::Menu& GetMenu() { return *this->_menu; }
        State GetState() const { return this->_state; }
        Tools::Thread::ThreadPool& GetThreadPool() { return *this->_threadPool; }
        Uint32 GetClientId() const { return this->_clientId; }
    private:
        void _MenuBind();
        void _ToggleDrawPhysicsBind();
    };

}

#endif
