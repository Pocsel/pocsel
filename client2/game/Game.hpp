#ifndef __CLIENT_GAME_GAME_HPP__
#define __CLIENT_GAME_GAME_HPP__

#include "client2/game/CubeTypeManager.hpp"

namespace Client {
    class Client;
}

namespace Client { namespace Game {

    class Game
        : private boost::noncopyable
    {
    private:
        Client& _client;
        CubeTypeManager _cubeTypeManager;

    public:
        Game(Client& client, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes);

        CubeTypeManager& GetCubeTypeManager() { return this->_cubeTypeManager; }
    };

}}

#endif
