#ifndef __CLIENT_GAME_GAME_HPP__
#define __CLIENT_GAME_GAME_HPP__

#include "common/Position.hpp"

#include "tools/Timer.hpp"
//#include "tools/lua/Interpreter.hpp"
#include "tools/stat/Timer.hpp"
#include "tools/gfx/utils/DeferredShading.hpp"

#include "client/game/CubeTypeManager.hpp"
#include "client/resources/ResourceManager.hpp"
#include "client/game/engine/Engine.hpp"

namespace Client {
    class Client;
    namespace Map {
        class Map;
    }
    namespace Game {
        class Player;
    }
}
namespace Tools {
    namespace Gfx {
        class IRenderer;
        class IRenderTarget;
        class IShaderParameter;
        namespace Effect {
            class Effect;
            class EffectManager;
        }
        namespace Utils {
            class GBuffer;
            class Image;
            class Sphere;
            namespace Light {
                class DirectionnalLight;
                class LightRenderer;
                class PointLight;
            }
        }
    }
}

namespace Client { namespace Game {

    class Game
        : private boost::noncopyable
    {
    private:
        Client& _client;
        Tools::Gfx::IRenderer& _renderer;
        Tools::Gfx::Effect::EffectManager& _effectManager;
        CubeTypeManager* _cubeTypeManager;
        Resources::ResourceManager* _resourceManager;
        Map::Map* _map;
        Player* _player;
        Engine::Engine* _engine;
        int _callbackId;
        Tools::Timer _updateTimer;
        Tools::Timer _gameTimer;
        Tools::Stat::Timer _statUpdateTime;
        Tools::Stat::Timer _statRenderTime;
        // XXX
        std::unique_ptr<Tools::Gfx::Utils::GBuffer> _gBuffer;
        std::unique_ptr<Tools::Gfx::Utils::Light::LightRenderer> _lightRenderer;
        std::list<Tools::Gfx::Utils::Light::DirectionnalLight> _directionnalLights;
        std::list<Tools::Gfx::Utils::Light::PointLight> _pointLights;

        Tools::Gfx::Utils::DeferredShading _deferredShading;

        std::unique_ptr<Tools::Gfx::Utils::Material::LuaMaterial> _postProcessSepia;
        //std::unique_ptr<Tools::Renderers::Utils::Image> _testImage;
        //Tools::Renderers::IShaderProgram* _testShader;
        //std::unique_ptr<Tools::Renderers::IShaderParameter> _testTexture;
        // XXX

    public:
        Game(Client& client,
                std::string const& worldIdentifier,
                std::string const& worldName,
                Uint32 worldVersion,
                Common::BaseChunk::CubeType nbCubeTypes,
                Uint32 nbBodyTypes,
                std::string const& worldBuildHash);
        ~Game();

        void TeleportPlayer(std::string const& map, Common::Position const& position);
        void LoadResources();
        void Update();
        void Render();

        Client& GetClient() { return this->_client; }
        Tools::Gfx::IRenderer& GetRenderer() { return this->_renderer; }
        Tools::Gfx::Effect::EffectManager& GetEffectManager() { return this->_effectManager; }
        Player& GetPlayer() { return *this->_player; }
        CubeTypeManager& GetCubeTypeManager() { return *this->_cubeTypeManager; }
        Resources::ResourceManager& GetResourceManager() { return *this->_resourceManager; }
        Map::Map& GetMap() { return *this->_map; }
        Tools::Lua::Interpreter& GetInterpreter() { return this->_engine->GetInterpreter(); }
        Engine::Engine& GetEngine() { return *this->_engine; }
        float GetLoadingProgression() const { return (this->_cubeTypeManager->GetLoadingProgression() + this->_resourceManager->GetLoadingProgression()) / 2.0f; }
    private:
        void _RenderScene(glm::dmat4 viewProjection);
    };

}}

#endif
