#include "client/precompiled.hpp"

#include "tools/IRenderer.hpp"
#include "tools/lua/utils/Utils.hpp"
#include "tools/renderers/utils/GBuffer.hpp"
#include "tools/renderers/utils/Image.hpp"
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
        _cubeTypeManager(client, nbCubeTypes),
        _map(0)
    {
        Tools::Lua::Utils::RegisterColor(this->_interpreter);
        this->_resourceManager = new Resources::ResourceManager(*this, client.GetNetwork().GetHost(), worldIdentifier, worldName, worldVersion, worldBuildHash);
        this->_renderer.SetClearColor(Tools::Color4f(120.f / 255.f, 153.f / 255.f, 201.f / 255.f, 1)); // XXX
        this->_itemManager = new ItemManager(*this),
        this->_player = new Player(*this);
        this->_callbackId = this->_client.GetWindow().RegisterCallback(
            [this](glm::uvec2 const& size)
            {
                this->GetPlayer().GetCamera().projection = glm::perspective<float>(90, size.x / float(size.y), 0.05f, 300.0f);
                this->_gBuffer->Resize(size);
            });
        auto const& size = this->_client.GetWindow().GetSize();
        this->GetPlayer().GetCamera().projection = glm::perspective<float>(90, size.x / float(size.y), 0.05f, 300.0f);
        // XXX
        this->_gBuffer = std::unique_ptr<Tools::Renderers::Utils::GBuffer>(new Tools::Renderers::Utils::GBuffer(this->_renderer, size));
        this->_renderImage = std::unique_ptr<Tools::Renderers::Utils::Image>(new Tools::Renderers::Utils::Image(this->_renderer));
        this->_renderShader = &this->_client.GetLocalResourceManager().GetShader("PostProcess.fx");
        this->_renderColorsParameter = this->_renderShader->GetParameter("colors");
        this->_renderNormalsParameter = this->_renderShader->GetParameter("normals");
        this->_renderDepthParameter = this->_renderShader->GetParameter("depthBuffer");

        this->_basicShader = &this->_client.GetLocalResourceManager().GetShader("BaseShaderTexture.fx");
        this->_basicParameter = this->_basicShader->GetParameter("baseTex");
        // XXX
    }

    Game::~Game()
    {
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
        Tools::Delete(this->_map);
        Tools::Delete(this->_player);
        Tools::Delete(this->_itemManager);
        Tools::Delete(this->_resourceManager);
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
        this->_map->GetChunkManager().Update(this->_gameTimer.GetPreciseElapsedTime(), this->_player->GetPosition());
        Uint32 time = this->_updateTimer.GetElapsedTime();
        this->_player->UpdateMovements(time);
        this->_itemManager->Update(time);
        this->_updateTimer.Reset();
    }

    void Game::Render()
    {
        this->_renderer.BeginDraw();

        this->_renderer.SetProjectionMatrix(this->GetPlayer().GetCamera().projection);
        this->_renderer.SetViewMatrix(this->GetPlayer().GetCamera().GetViewMatrix());

        this->_gBuffer->Bind();
        this->_renderer.Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth);

        this->_map->GetChunkManager().Render();
        this->_itemManager->Render();
        this->_map->GetChunkManager().RenderAlpha();
        this->_player->Render();

        this->_gBuffer->Unbind();

        // XXX
        this->_renderer.BeginDraw2D();
        auto const& size = this->GetClient().GetWindow().GetSize();
        this->_renderer.SetModelMatrix(glm::scale<float>(size.x / 2.0f, size.y / 2.0f, 1) * glm::translate<float>(1, 1, 0));

        this->_renderShader->BeginPass();
        this->_gBuffer->GetNormals().Bind();
        this->_gBuffer->GetDepth().Bind();
        this->_renderNormalsParameter->Set(this->_gBuffer->GetNormals());
        this->_renderDepthParameter->Set(this->_gBuffer->GetDepth());
        this->_renderImage->Render(*this->_renderColorsParameter, this->_gBuffer->GetColors());
        this->_gBuffer->GetDepth().Unbind();
        this->_gBuffer->GetNormals().Unbind();
        this->_renderShader->EndPass();

        this->_basicShader->BeginPass();
        this->_renderer.SetModelMatrix(glm::scale<float>(64, 64, 1) * glm::translate<float>(1, 1, 0));
        this->_renderImage->Render(*this->_basicParameter, this->_gBuffer->GetNormals());
        this->_renderer.SetModelMatrix(glm::scale<float>(64, 64, 1) * glm::translate<float>(3, 1, 0));
        this->_renderImage->Render(*this->_basicParameter, this->_gBuffer->GetDepth());
        this->_renderer.SetModelMatrix(glm::scale<float>(64, 64, 1) * glm::translate<float>(5, 1, 0));
        this->_renderImage->Render(*this->_basicParameter, this->_gBuffer->GetColors());
        this->_basicShader->EndPass();
        
        this->_renderer.EndDraw2D();
        // XXX

        this->_renderer.EndDraw();
    }

}}
