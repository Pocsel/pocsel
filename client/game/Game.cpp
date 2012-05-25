#include "client/precompiled.hpp"

#include "tools/IRenderer.hpp"
#include "tools/lua/utils/Utils.hpp"
#include "tools/renderers/utils/GBuffer.hpp"
#include "tools/renderers/utils/Image.hpp"
#include "tools/renderers/utils/LightRenderer.hpp"
#include "tools/stat/StatManager.hpp"
#include "tools/window/Window.hpp"

#include "client/Client.hpp"
#include "client/network/Network.hpp"
#include "client/game/Game.hpp"
#include "client/map/Map.hpp"
#include "client/game/Player.hpp"
#include "client/game/ItemManager.hpp"
#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Game {

    Game::Game(Client& client, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes, std::string const& worldBuildHash) :
        _client(client),
        _renderer(client.GetWindow().GetRenderer()),
        _map(0),
        _statUpdateTime("Game update"),
        _statRenderTime("Game render"),
        _statOutTime("Not game time")
    {
        this->_cubeTypeManager = new CubeTypeManager(client, nbCubeTypes);
        this->_resourceManager = new Resources::ResourceManager(*this, client.GetNetwork().GetHost(), worldIdentifier, worldName, worldVersion, worldBuildHash);
        this->_renderer.SetClearColor(Tools::Color4f(120.f / 255.f, 153.f / 255.f, 201.f / 255.f, 1)); // XXX
        this->_itemManager = new ItemManager(*this),
        this->_player = new Player(*this);
        this->_engine = new Engine::Engine(*this);
        this->_callbackId = this->_client.GetWindow().RegisterCallback(
            [this](glm::uvec2 const& size)
            {
                this->GetPlayer().GetCamera().projection = glm::perspective<float>(90, size.x / float(size.y), 0.05f, 300.0f);
                this->_gBuffer->Resize(size);
            });
        auto const& size = this->_client.GetWindow().GetSize();
        this->GetPlayer().GetCamera().projection = glm::perspective<float>(90, size.x / float(size.y), 0.05f, 300.0f);
        // XXX
        this->_gBuffer = std::unique_ptr<Tools::Renderers::Utils::GBuffer>(
            new Tools::Renderers::Utils::GBuffer(
                this->_renderer,
                size,
                this->_client.GetLocalResourceManager().GetShader("PostProcess.fx")));
        this->_lightRenderer.reset(
            new Tools::Renderers::Utils::LightRenderer(
                this->_renderer,
                this->_client.GetLocalResourceManager().GetShader("DirectionnalDepth.fx"),
                this->_client.GetLocalResourceManager().GetShader("DirectionnalLight.fx"),
                this->_client.GetLocalResourceManager().GetShader("PointLight.fx")));
        this->_directionnalLights.push_back(this->_lightRenderer->CreateDirectionnalLight());
        this->_directionnalLights.back().direction = glm::normalize(glm::vec3(0.5, -1, 0.5));
        this->_directionnalLights.back().ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
        this->_directionnalLights.back().diffuseColor =  glm::vec3(1.0f, 1.0f, 1.0f);
        this->_directionnalLights.back().specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

        //this->_pointLights.push_back(this->_lightRenderer->CreatePointLight());
        //this->_pointLights.back().position = glm::vec3(.0f, .0f, .0f);
        //this->_pointLights.back().range = 20.0f;
        //this->_pointLights.back().diffuseColor =  glm::vec3(0.8f, 0.9f, 1.0f);
        //this->_pointLights.back().specularColor = glm::vec3(0.8f, 0.9f, 1.0f);
        // XXX
    }

    Game::~Game()
    {
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
        Tools::Delete(this->_map);
        Tools::Delete(this->_player);
        Tools::Delete(this->_itemManager);
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

        this->_map->GetChunkManager().Update(this->_gameTimer.GetPreciseElapsedTime(), this->_player->GetPosition());
        Uint32 time = this->_updateTimer.GetElapsedTime();
        this->_player->UpdateMovements(time);
        this->_itemManager->Update(time);
        this->_updateTimer.Reset();

        this->_statUpdateTime.End();
    }

    void Game::Render()
    {
        this->_statRenderTime.Begin();

        auto& camera = this->GetPlayer().GetCamera();
        this->_renderer.SetProjectionMatrix(camera.projection);
        this->_renderer.SetViewMatrix(camera.GetViewMatrix());

        auto viewProjection = glm::dmat4x4(camera.projection)
            * glm::lookAt(camera.position, camera.position + glm::dvec3(camera.direction), glm::dvec3(0, 1, 0));

        this->_renderer.BeginDraw();

        this->_gBuffer->Bind();
        this->_renderer.Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth | Tools::ClearFlags::Stencil);

        this->_RenderScene(viewProjection);
        this->_map->GetChunkManager().RenderAlpha(this->GetPlayer().GetCamera().position);
        this->_player->Render();

        this->_gBuffer->Unbind();

        std::function<void(glm::dmat4)> renderScene = std::bind(&Game::_RenderScene, this, std::placeholders::_1);
        this->_renderer.Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth | Tools::ClearFlags::Stencil);
        this->_lightRenderer->Render(
            *this->_gBuffer,
            Tools::Frustum(viewProjection),
            renderScene,
            this->_directionnalLights,
            this->_pointLights);
        this->_gBuffer->Render();

        this->_renderer.EndDraw();

        this->_statRenderTime.End();
    }

    void Game::LoadResources()
    {
        try
        {
            this->_engine->LoadLuaScripts(); // registering de tous les registrables client (Effect, CubeMaterial, ...)
            this->_resourceManager->BuildResourceIndex(); // créé des maps pour acceder au resources par pluginId + nom
            this->_cubeTypeManager->LoadMaterials(); // construit les cubes types à partir des CubeMaterial registerés
        }
        catch (std::exception& e)
        {
            this->_client.Disconnect(e.what());
        }
    }

    void Game::_RenderScene(glm::dmat4 viewProjection)
    {
        this->_map->GetChunkManager().Render(this->GetPlayer().GetCamera().position, viewProjection);
        this->_itemManager->Render();
    }

}}
