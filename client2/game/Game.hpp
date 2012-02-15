#ifndef __CLIENT_GAME_GAME_HPP__
#define __CLIENT_GAME_GAME_HPP__

#include "client2/game/CubeTypeManager.hpp"
#include "client2/resources/ResourceManager.hpp"

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

    public:
        Game(Client& client, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes);
        ~Game();

        void Update();
        void Render();

        Client& GetClient() { return this->_client; }
        CubeTypeManager& GetCubeTypeManager() { return this->_cubeTypeManager; }
        Resources::ResourceManager& GetResourceManager() { return this->_resourceManager; }
        float GetLoadingProgression() const { return (this->_cubeTypeManager.GetLoadingProgression() + this->_resourceManager.GetLoadingProgression()) / 2.0f; }
    };

}}

#endif
