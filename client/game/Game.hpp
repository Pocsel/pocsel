#ifndef __CLIENT_GAME_GAME_HPP__
#define __CLIENT_GAME_GAME_HPP__

#include "common/Position.hpp"

#include "tools/Timer.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/stat/Timer.hpp"

#include "client/game/CubeTypeManager.hpp"
#include "client/resources/ResourceManager.hpp"

namespace Client {
    class Client;
    namespace Map {
        class Map;
    }
    namespace Game {
        class Player;
        class ItemManager;
    }
}
namespace Tools {
    class IRenderer;
    namespace Renderers {
        class IRenderTarget;
        class IShaderProgram;
        class IShaderParameter;
        namespace Utils {
            class DirectionnalLight;
            class GBuffer;
            class Image;
            class LightRenderer;
            class PointLight;
        }
    }
}

namespace Client { namespace Game {

    class Game
        : private boost::noncopyable
    {
    private:
        Client& _client;
        Tools::IRenderer& _renderer;
        CubeTypeManager _cubeTypeManager;
        Resources::ResourceManager* _resourceManager;
        Map::Map* _map;
        ItemManager* _itemManager;
        Player* _player;
        int _callbackId;
        Tools::Timer _updateTimer;
        Tools::Timer _gameTimer;
        Tools::Lua::Interpreter _interpreter;
        Tools::Stat::Timer _statUpdateTime;
        Tools::Stat::Timer _statRenderTime;
        Tools::Stat::Timer _statOutTime;
        // XXX
        std::unique_ptr<Tools::Renderers::Utils::GBuffer> _gBuffer;
        std::unique_ptr<Tools::Renderers::Utils::LightRenderer> _lightRenderer;
        std::list<Tools::Renderers::Utils::DirectionnalLight> _directionnalLights;
        std::list<Tools::Renderers::Utils::PointLight> _pointLights;
        // XXX

    public:
        Game(Client& client, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes, std::string const& worldBuildHash);
        ~Game();

        void TeleportPlayer(std::string const& map, Common::Position const& position);
        void Update();
        void Render();

        Client& GetClient() { return this->_client; }
        Tools::IRenderer& GetRenderer() { return this->_renderer; }
        Player& GetPlayer() { return *this->_player; }
        CubeTypeManager& GetCubeTypeManager() { return this->_cubeTypeManager; }
        Resources::ResourceManager& GetResourceManager() { return *this->_resourceManager; }
        ItemManager& GetItemManager() { return *this->_itemManager; }
        Map::Map& GetMap() { return *this->_map; }
        Tools::Lua::Interpreter& GetInterpreter() { return this->_interpreter; }
        float GetLoadingProgression() const { return (this->_cubeTypeManager.GetLoadingProgression() + this->_resourceManager->GetLoadingProgression()) / 2.0f; }
    };

}}

#endif
