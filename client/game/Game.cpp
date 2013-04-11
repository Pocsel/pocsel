#include "client/precompiled.hpp"

#include "tools/lua/utils/Utils.hpp"
#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/effect/EffectManager.hpp"
#include "tools/gfx/utils/GBuffer.hpp"
#include "tools/gfx/utils/Image.hpp"
#include "tools/gfx/utils/light/LightRenderer.hpp"
#include "tools/stat/StatManager.hpp"
#include "tools/window/Window.hpp"

#include "client/Client.hpp"
#include "client/network/Network.hpp"
#include "client/game/Game.hpp"
#include "client/map/Map.hpp"
#include "client/game/Player.hpp"
#include "client/game/engine/ModelManager.hpp"
#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Game {

    Game::Game(Client& client,
            std::string const& worldIdentifier,
            std::string const& worldName,
            Uint32 worldVersion,
            Common::BaseChunk::CubeType nbCubeTypes,
            Uint32 nbBodyTypes,
            std::string const& worldBuildHash) :
        _client(client),
        _renderer(client.GetWindow().GetRenderer()),
        _effectManager(client.GetEffectManager()),
        _map(0),
        _soundSystem(client.GetSoundSystem()),
        _statUpdateTime("Game update"),
        _statRenderTime("Game render"),
        _deferredShading(client.GetWindow().GetRenderer())
    {
        this->_cubeTypeManager = new CubeTypeManager(client, nbCubeTypes);
        this->_resourceManager = new Resources::ResourceManager(*this, client.GetNetwork().GetHost(), worldIdentifier, worldName, worldVersion, worldBuildHash);
        this->_renderer.SetClearColor(glm::vec4(120.f / 255.f, 153.f / 255.f, 201.f / 255.f, 1)); // XXX
        this->_player = new Player(*this);
        this->_engine = new Engine::Engine(*this, nbBodyTypes);
        this->_callbackId = this->_client.GetWindow().RegisterCallback(
            [this](glm::uvec2 const& size)
            {
                this->GetPlayer().GetCamera().projection = glm::perspective<float>(90, size.x / float(size.y), 0.05f, 300.0f);
                this->_gBuffer->Resize(size);
            });
        auto const& size = this->_client.GetWindow().GetSize();
        this->GetPlayer().GetCamera().projection = glm::perspective<float>(90, size.x / float(size.y), 0.05f, 300.0f);
        // XXX
        this->_gBuffer = std::unique_ptr<Tools::Gfx::Utils::GBuffer>(
            new Tools::Gfx::Utils::GBuffer(
                this->_renderer,
                size,
                this->_client.GetLocalResourceManager().GetShader("PostProcess.fxc")));
        this->_lightRenderer.reset(
            new Tools::Gfx::Utils::Light::LightRenderer(
                this->_renderer,
                this->_client.GetLocalResourceManager().GetShader("DirectionnalLight.fxc"),
                this->_client.GetLocalResourceManager().GetShader("PointLight.fxc")));
        this->_directionnalLights.push_back(this->_lightRenderer->CreateDirectionnalLight());
        this->_directionnalLights.back().direction = glm::normalize(glm::vec3(0.5, -1, 0.5));
        this->_directionnalLights.back().ambientColor = glm::vec4(0.1f, 0.1f, 0.1f, 0.1f);
        this->_directionnalLights.back().diffuseColor =  glm::vec3(1.0f, 1.0f, 1.0f);
        this->_directionnalLights.back().specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

        this->_pointLights.push_back(this->_lightRenderer->CreatePointLight());
        this->_pointLights.back().position = glm::vec3(.0f, .0f, .0f);
        this->_pointLights.back().range = 32.0f;
        this->_pointLights.back().diffuseColor =  glm::vec3(0.8f, 0.75f, 0.4f);
        this->_pointLights.back().specularColor = glm::vec3(.0f, .0f, .0f);

        //this->_testImage.reset(new Tools::Gfx::Utils::Image(this->_renderer));
        //this->_testShader = &this->_client.GetLocalResourceManager().GetShader("BaseShaderTexture.fx");
        //this->_testTexture = this->_testShader->GetParameter("baseTex");
        // XXX
    }

    Game::~Game()
    {
        this->_postProcessSepia.reset();
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
        Tools::Delete(this->_map);
        Tools::Delete(this->_player);
        Tools::Delete(this->_resourceManager);
        Tools::Delete(this->_cubeTypeManager);
        Tools::Delete(this->_engine);
    }

    void Game::TeleportPlayer(std::string const& map, Common::Position const& position)
    {
        this->_player->SetPosition(position);
        if (this->_map != 0 && this->_map->GetName() != map)
        {
            Tools::Delete(this->_map);
            this->_map = 0;
        }
        if (this->_map == 0)
        {
            this->_map = new Map::Map(*this, map);
            this->_map->GetChunkManager().Update(0, this->_player->GetPosition());
            this->_client.LoadChunks();
        }
    }

    void Game::Update()
    {
        this->_statUpdateTime.Begin();

        Uint64 totalTime = this->_gameTimer.GetPreciseElapsedTime();
        this->_map->GetChunkManager().Update(totalTime, this->_player->GetPosition());
        this->_player->UpdateMovements(this->_updateTimer.GetPreciseElapsedTime());
        this->_engine->Tick(totalTime);
        this->_updateTimer.Reset();

        this->_statUpdateTime.End();
    }

    void Game::Render()
    {
        this->_statRenderTime.Begin();

        Uint64 totalTime = this->_gameTimer.GetPreciseElapsedTime();
        auto& camera = this->GetPlayer().GetCamera();
        auto const& viewMatrix = camera.GetViewMatrix();
        this->_renderer.SetProjectionMatrix(camera.projection);
        this->_renderer.SetViewMatrix(viewMatrix);

        auto absoluteViewProjection = camera.GetAbsoluteViewProjectionMatrix();

        this->_renderer.BeginDraw();
        //this->_renderer.Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth | Tools::ClearFlags::Stencil);

        this->_gBuffer->Bind();
        this->_renderer.Clear(Tools::Gfx::ClearFlags::Color | Tools::Gfx::ClearFlags::Depth | Tools::Gfx::ClearFlags::Stencil);

        //this->_RenderScene(absoluteViewProjection);
        this->_map->GetChunkManager().Render(this->_deferredShading, this->GetPlayer().GetCamera().position, absoluteViewProjection);
        this->_engine->GetModelManager().Render(this->_deferredShading);

        this->_deferredShading.RenderGeometry(totalTime);

        this->_player->Render();
        this->_engine->GetDoodadManager().Render();

        this->_gBuffer->Unbind();

        // XXX
        //this->_pointLights.front().position = glm::vec3(glm::dvec3(67108864.0, 16777216.0, 67108864.0) - camera.position); // au spawn
        this->_pointLights.front().position = camera.direction * 1.0f; // ~1 cube devant la caméra
        std::function<void(glm::dmat4)> renderScene = std::bind(&Game::_RenderScene, this, std::placeholders::_1);
        this->_lightRenderer->Render(
            *this->_gBuffer,
            viewMatrix,
            camera.projection,
            Tools::Frustum(absoluteViewProjection),
            camera.position,
            renderScene,
            this->_directionnalLights,
            this->_pointLights);
        // XXX

        std::list<Tools::Gfx::Utils::Material::LuaMaterial*> postProcess;
        //if (this->_postProcessSepia == 0)
        //    this->_postProcessSepia = this->_resourceManager->GetMaterial("base:PostProcessSepia");
        //postProcess.push_back(this->_postProcessSepia.get());
        this->_gBuffer->Render(totalTime, postProcess);

        this->_renderer.EndDraw();
        // XXX
        //this->_renderer.BeginDraw2D();
        //this->_renderer.SetModelMatrix(glm::scale(256.0f, 256.0f, 1.0f) * glm::translate(0.5f, 0.5f, 0.0f));
        //do
        //{
        //    this->_testShader->BeginPass();
        //    this->_testImage->Render(*this->_testTexture, this->_lightRenderer->GetDirectionnalShadowMap(0));
        //} while (this->_testShader->EndPass());
        //this->_renderer.EndDraw2D();
        // XXX

        this->_statRenderTime.End();
    }

    void Game::LoadResources()
    {
        try
        {
            this->_resourceManager->BuildResourceIndex(); // créé des maps pour acceder au resources par pluginId + nom
            this->_engine->LoadLuaScripts(); // registering de tous les registrables client (Effect, CubeMaterial, ...)
            this->_cubeTypeManager->LoadMaterials(); // construit les cubes types à partir des CubeMaterial registerés
        }
        catch (std::exception& e)
        {
            this->_client.Disconnect(e.what());
        }
    }

    void Game::_RenderScene(glm::dmat4 viewProjection)
    {
    }

}}
