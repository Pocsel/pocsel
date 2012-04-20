#include "client/precompiled.hpp"

#include "tools/IRenderer.hpp"
#include "tools/lua/utils/Utils.hpp"
#include "tools/renderers/utils/Image.hpp"

#include "client/Client.hpp"
#include "client/network/Network.hpp"
#include "client/game/Game.hpp"
#include "client/map/Map.hpp"
#include "client/window/Window.hpp"
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
                this->GetPlayer().GetCamera().projection = glm::perspective<float>(90, size.x / float(size.y), 0.02f, 500.0f);
                this->_renderTarget->Resize(size);
            });
        auto const& size = this->_client.GetWindow().GetSize();
        this->GetPlayer().GetCamera().projection = glm::perspective<float>(90, size.x / float(size.y), 0.02f, 500.0f);
        // XXX
        this->_renderTarget = this->_renderer.CreateRenderTarget(glm::uvec2(size.x, size.y));
        this->_renderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Depth24Stencil8, Tools::Renderers::RenderTargetUsage::DepthStencil);
        Tools::log << " - " << this->_renderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba8, Tools::Renderers::RenderTargetUsage::Color) << std::endl;
        Tools::log << " - " << this->_renderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba8, Tools::Renderers::RenderTargetUsage::Color) << std::endl;
        this->_renderImage = std::unique_ptr<Tools::Renderers::Utils::Image>(new Tools::Renderers::Utils::Image(this->_renderer));
        this->_renderShader = &this->_client.GetLocalResourceManager().GetShader("PostProcess.cgfx");
        this->_renderParameter = this->_renderShader->GetParameter("baseTex");
        this->_renderTimeParameter = this->_renderShader->GetParameter("time");
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
        // XXX
        //this->_renderer.BeginDraw(this->_renderTarget.get());
        //this->_renderer.SetProjectionMatrix(glm::perspective<float>(90, 1, 0.02f, 500.0f));
        //this->_renderer.SetViewMatrix(glm::lookAt<float>(this->GetPlayer().GetCamera().direction * -50.0f, this->GetPlayer().GetCamera().direction, glm::fvec3(0, 1, 0)));
        //this->_renderer.Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth);
        //this->_map->GetChunkManager().Render();
        //this->_renderer.EndDraw();
        // XXX

        this->_renderer.SetProjectionMatrix(this->GetPlayer().GetCamera().projection);
        this->_renderer.SetViewMatrix(this->GetPlayer().GetCamera().GetViewMatrix());

        //this->_renderer.Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth);
        this->_renderer.BeginDraw(this->_renderTarget.get());
        this->_renderer.Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth);

        //this->_renderer.SetDepthTest(true);
        this->_map->GetChunkManager().Render();
        this->_itemManager->Render();
        this->_map->GetChunkManager().RenderAlpha();
        this->_player->Render();

        this->_renderer.EndDraw();

        // XXX
        this->_renderer.BeginDraw2D();
        auto const& size = this->GetClient().GetWindow().GetSize();
        this->_renderer.SetModelMatrix(glm::scale<float>(size.x / 2.0f, size.y / 2.0f, 1) * glm::translate<float>(1, 1, 0));
        this->_renderShader->BeginPass();
        this->_renderTimeParameter->Set((float)this->_gameTimer.GetPreciseElapsedTime() * 0.000001f);
        this->_renderImage->Render(*this->_renderParameter, this->_renderTarget->GetTexture(1));
        this->_renderer.SetModelMatrix(glm::scale<float>(128, 128, 1) * glm::translate<float>(1, 1, 0));
        this->_renderImage->Render(*this->_renderParameter, this->_renderTarget->GetTexture(2));
        this->_renderer.SetModelMatrix(glm::scale<float>(128, 128, 1) * glm::translate<float>(3, 1, 0));
        this->_renderImage->Render(*this->_renderParameter, this->_renderTarget->GetTexture(0));
        this->_renderShader->EndPass();
        this->_renderer.EndDraw2D();
        // XXX

        this->_renderer.EndDraw();
    }

}}
