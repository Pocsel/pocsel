#include "client2/precompiled.hpp"

#include "tools/IRenderer.hpp"
#include "client2/Client.hpp"
#include "client2/network/Network.hpp"
#include "client2/game/Game.hpp"
#include "client2/map/Map.hpp"
#include "client2/window/Window.hpp"

namespace Client { namespace Game {

    Game::Game(Client& client, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes)
        : _client(client),
        _renderer(client.GetWindow().GetRenderer()),
        _cubeTypeManager(client, nbCubeTypes),
        _resourceManager(client, client.GetNetwork().GetHost(), worldIdentifier, worldName, worldVersion),
        _map(0)
    {
        this->_player.GetCamera().Rotate(0, 0);
        this->_renderer.SetClearColor(Tools::Color4f(0.0f, 0.0f, 0.1f, 1)); // XXX
        this->_callbackId = this->_client.GetWindow().RegisterCallback(
            [this](Tools::Vector2u const& size)
            {
                this->GetPlayer().GetCamera().projection = Tools::Matrix4<float>::CreatePerspective(90, size.w / float(size.h), 0.001f, 500.0f);
            });
        this->GetPlayer().GetCamera().projection = Tools::Matrix4<float>::CreatePerspective(90, this->_client.GetWindow().GetSize().w / float(this->_client.GetWindow().GetSize().h), 0.001f, 500.0f);
    }

    Game::~Game()
    {
        this->_client.GetWindow().UnregisterCallback(this->_callbackId);
        Tools::Delete(this->_map);
    }

    void Game::TeleportPlayer(std::string const& map, Common::Position const& position)
    {
        // TODO: player
        //this->_player.SetPosition(position);
        // XXX
        auto pos(position);
        pos.world.y += 3;
        this->_player.SetPosition(pos);
        // XXX
        if (this->_map == 0)
        {
            this->_map = new Map::Map(*this);
            this->_map->GetChunkManager().Update(this->_player.GetPosition());
            this->_client.LoadChunks();
        }
    }

    void Game::Update()
    {
        this->_player.UpdateMovements(this->_client.GetWindow());
        this->_map->GetChunkManager().Update(this->_player.GetPosition());
    }

    void Game::Render()
    {
        this->_renderer.Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth);
        this->_renderer.SetProjectionMatrix(this->GetPlayer().GetCamera().projection);
        this->_renderer.SetViewMatrix(this->GetPlayer().GetCamera().GetViewMatrix());
        this->_renderer.BeginDraw();
        this->_map->GetChunkManager().Render();
        this->_renderer.EndDraw();
    }

}}
