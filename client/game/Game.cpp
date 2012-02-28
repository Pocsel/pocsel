#include "client/precompiled.hpp"

#include "tools/IRenderer.hpp"
#include "client/Client.hpp"
#include "client/network/Network.hpp"
#include "client/game/Game.hpp"
#include "client/map/Map.hpp"
#include "client/window/Window.hpp"
#include "client/game/Player.hpp"

namespace Client { namespace Game {

    Game::Game(Client& client, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes) :
        _client(client),
        _renderer(client.GetWindow().GetRenderer()),
        _cubeTypeManager(client, nbCubeTypes),
        _map(0)
    {
        this->_resourceManager = new Resources::ResourceManager(*this, client.GetNetwork().GetHost(), worldIdentifier, worldName, worldVersion);
        this->_renderer.SetClearColor(Tools::Color4f(0.0f, 0.0f, 0.0f, 1)); // XXX
        this->_player = new Player(*this);
        this->_callbackId = this->_client.GetWindow().RegisterCallback(
            [this](Tools::Vector2u const& size)
            {
                this->GetPlayer().GetCamera().projection = Tools::Matrix4<float>::CreatePerspective(90, size.w / float(size.h), 0.02f, 500.0f);
            });
        this->GetPlayer().GetCamera().projection = Tools::Matrix4<float>::CreatePerspective(90, this->_client.GetWindow().GetSize().w / float(this->_client.GetWindow().GetSize().h), 0.02f, 500.0f);
    }

    Game::~Game()
    {
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
        Tools::Delete(this->_map);
        Tools::Delete(this->_player);
        Tools::Delete(this->_resourceManager);
    }

    void Game::TeleportPlayer(std::string const& map, Common::Position const& position)
    {
        this->_player->SetPosition(position);
        if (this->_map != 0 && this->_map->GetName() != map)
        {
            delete this->_map;
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
        this->_player->UpdateMovements(this->_updateTimer.GetElapsedTime());
        this->_updateTimer.Reset();
    }

    void Game::Render()
    {
        this->_renderer.Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth);
        this->_renderer.SetProjectionMatrix(this->GetPlayer().GetCamera().projection);
        this->_renderer.SetViewMatrix(this->GetPlayer().GetCamera().GetViewMatrix());
        this->_renderer.BeginDraw();
        this->_map->GetChunkManager().Render();
        this->_player->Render();
        this->_renderer.EndDraw();
    }

}}
