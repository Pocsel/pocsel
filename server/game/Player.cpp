#include "server/game/Player.hpp"

#include "server/game/map/Map.hpp"

namespace Server { namespace Game {

    Player::Player(Game& game, Uint32 id, std::string const& playerName, Uint32 viewDistance) :
        id(id),
        _game(game),
        _playerName(playerName),
        _viewDistance(viewDistance),
        _currentMap(0),
        _inGame(false)
    {
    }

    Player::~Player()
    {
    }

    void Player::Teleport(Map::Map& map, Common::Position const& position)
    {
        if (this->_inGame)
            this->_currentMap->RemovePlayer(this->id);
        this->SetCurrentMap(map);
        this->_position = position;
        this->_inGame = false;
    }

    void Player::TeleportOk()
    {
        this->_currentMap->AddPlayer(this->shared_from_this());
        this->_inGame = true;
    }

    void Player::RemoveFromMap()
    {
        if (this->_inGame)
            this->_currentMap->RemovePlayer(this->id);
    }

    void Player::Move(Common::Camera const& cam, Tools::Vector3f const& movement)
    {

    }

}}
