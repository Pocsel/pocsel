#ifndef __CLIENT_CLIENT_HPP__
#define __CLIENT_CLIENT_HPP__

#include "common/BaseChunk.hpp"
#include "client2/game/Game.hpp"
#include "client2/network/Network.hpp"

namespace Client {
    class Settings;
    namespace Game {
        class CubeTypeManager;
    }
    namespace Network {
        class PacketDispatcher;
    }
    namespace Window {
        class Window;
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
            LoadingResources,
            LoadingChunks,
            Running,
            Disconnected,
        };

    private:
        Settings& _settings;
        Window::Window* _window;
        Network::Network _network;
        Network::PacketDispatcher* _packetDispatcher;
        State _state;
        Game::Game* _game;

    public:
        Client(Settings&& settings);
        ~Client();
        int Run();
        void Login(std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes);
        void Disconnect(std::string const& reason);
        void Quit();

        Settings& GetSettings() { return this->_settings; }
        Window::Window& GetWindow() { return *this->_window; }
        Network::Network& GetNetwork() { return this->_network; }
        Game::Game& GetGame() { return *this->_game; }
        State GetState() const { return this->_state; }
    };

}

#endif
