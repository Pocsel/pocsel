#ifndef __SERVER_GAME_PLAYER_HPP__
#define __SERVER_GAME_PLAYER_HPP__

#include "common/MovingOrientedPosition.hpp"

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
        Common::MovingOrientedPosition _position;

    public:
        Player(Game& game, Uint32 id, std::string const& playerName, Uint32 viewDistance);
        ~Player();

        Uint32 GetViewDistance() const { return this->_viewDistance; }
        void SetCurrentMap(Map::Map& map) { this->_currentMap = &map; }
        bool HasMap() const { return this->_currentMap != 0; }
        Map::Map& GetCurrentMap() { assert(this->_currentMap); return *this->_currentMap; }
        bool IsInGame() const { return this->_inGame; }
        void Teleport(Map::Map& map, Common::Position const& position);
        void TeleportOk();
        void RemoveFromMap();
        void SetPosition(Common::MovingOrientedPosition const& pos) { this->_position = pos; }
        Common::MovingOrientedPosition const& GetPosition() const { return this->_position; }
    };

}}

#endif
