#include "server2/game/Player.hpp"
#include "server2/game/Game.hpp"

namespace Server { namespace Game {

    Player::Player(Game& game, Uint32 id) :
        id(id),
        _game(game)
    {
    }

    Player::~Player()
    {
    }

    void Player::SetCurrentMap(Map::Map* map)
    {
        this->_currentMap = map;
    }

    Map::Map* Player::GetCurrentMap()
    {
        return this->_currentMap;
    }

    void Player::Teleport(Common::Position const& position)
    {
        // TODO state 'en teleportation'
    }

}}
