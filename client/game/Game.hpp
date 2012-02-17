#ifndef __CLIENT_GAME_GAME_HPP__
#define __CLIENT_GAME_GAME_HPP__

#include "common/Position.hpp"
#include "client/game/CubeTypeManager.hpp"
#include "client/game/Player.hpp"
#include "client/resources/ResourceManager.hpp"

namespace Client {
    class Client;
    namespace Map {
        class Map;
    }
}
namespace Tools {
    class IRenderer;
}

namespace Client { namespace Game {

    class Game
        : private boost::noncopyable
    {
    private:
        Client& _client;
        Tools::IRenderer& _renderer;
        CubeTypeManager _cubeTypeManager;
        Resources::ResourceManager _resourceManager;
        Map::Map* _map;
        Player _player;
        int _callbackId;

    public:
        Game(Client& client, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes);
        ~Game();

        void TeleportPlayer(std::string const& map, Common::Position const& position);
        void Update();
        void Render();

        Client& GetClient() { return this->_client; }
        Player& GetPlayer() { return this->_player; }
        CubeTypeManager& GetCubeTypeManager() { return this->_cubeTypeManager; }
        Resources::ResourceManager& GetResourceManager() { return this->_resourceManager; }
        Map::Map& GetMap() { return *this->_map; }
        float GetLoadingProgression() const { return (this->_cubeTypeManager.GetLoadingProgression() + this->_resourceManager.GetLoadingProgression()) / 2.0f; }
    };

}}

#endif
