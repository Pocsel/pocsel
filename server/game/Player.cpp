#include "server/game/Player.hpp"
//#include "server/game/Game.hpp"

namespace Server { namespace Game {

    Player::Player(Game& game, Uint32 id) :
        id(id),
        _game(game),
        _currentMap(0)
    {
    }

    Player::~Player()
    {
    }

    void Player::Teleport(Common::Position const& position)
    {
        // TODO state 'en teleportation'
    }

}}
