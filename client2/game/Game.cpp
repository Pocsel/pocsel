#include "client2/Client.hpp"
#include "client2/game/Game.hpp"

namespace Client { namespace Game {

    Game::Game(Client& client, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes)
        : _client(client),
        _cubeTypeManager(client, nbCubeTypes)
    {
    }

}}
