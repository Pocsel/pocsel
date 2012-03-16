#ifndef __SERVER_GAME_PLAYER_HPP__
#define __SERVER_GAME_PLAYER_HPP__

#include "common/Position.hpp"

namespace Server { namespace Game { namespace Map {

    class Map;

}}}

namespace Server { namespace Game {

    class Game;

    class Player :
        public std::enable_shared_from_this<Player>,
        private boost::noncopyable
    {
    public:
        Uint32 const id;
    private:
        Game& _game;
        std::string _playerName;
        Uint32 _viewDistance;
        Map::Map* _currentMap;
        bool _inGame;
        Common::MovingCamera _position;

    public:
        Player(Game& game, Uint32 id, std::string const& playerName, Uint32 viewDistance);
        ~Player();

        void SetCurrentMap(Map::Map& map) { this->_currentMap = &map; }
        bool HasMap() const { return this->_currentMap != 0; }
        Map::Map& GetCurrentMap() { assert(this->_currentMap); return *this->_currentMap; }
        Common::Position const& GetPosition() const { return this->_position; }
        bool IsInGame() const { return this->_inGame; }
        void Teleport(Map::Map& map, Common::Position const& position);
        void TeleportOk();
        void RemoveFromMap();
        void Player::Move(Common::Camera const& cam, Tools::Vector3f const& movement);
    };

}}

#endif
