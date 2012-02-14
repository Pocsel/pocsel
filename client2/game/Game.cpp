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
    }

    Game::~Game()
    {
        Tools::Delete(this->_map);
    }

    void Game::Update()
    {
    }

    void Game::Render()
    {
        this->_renderer.BeginDraw();
        this->_renderer.EndDraw();
    }

}}
