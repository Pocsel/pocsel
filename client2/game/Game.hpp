#ifndef __CLIENT_GAME_GAME_HPP__
#define __CLIENT_GAME_GAME_HPP__

#include "client2/game/CubeTypeManager.hpp"
#include "client2/resources/ResourceManager.hpp"

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
        Resources::ResourceManager _resourceManager;

    public:
        Game(Client& client, std::string const& host, std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes);

        CubeTypeManager& GetCubeTypeManager() { return this->_cubeTypeManager; }
        Resources::ResourceManager& GetResourceManager() { return this->_resourceManager; }
    };

}}

#endif
