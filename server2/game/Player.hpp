#ifndef __SERVER_GAME_PLAYER_HPP__
#define __SERVER_GAME_PLAYER_HPP__

namespace Common {

    struct Position;

}

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
        typedef std::function<void(Common::Position const&)> TeleportCallback;

    public:
        Uint32 const id;
    private:
        Game& _game;
        Map::Map* _currentMap;
        TeleportCallback _teleportCallback;

    public:
        Player(Game& game, Uint32 id);
        ~Player();

        void SetCurrentMap(Map::Map* map);
        Map::Map* GetCurrentMap();
        void Teleport(Common::Position const& position);
    };

}}

#endif