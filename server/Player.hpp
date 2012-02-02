#ifndef __SERVER_PLAYER_HPP__
#define __SERVER_PLAYER_HPP__

#include "server/Map.hpp"

namespace Server {

    class Player
    {
    private:
        Map*        _map;

    public:
        Player(Map& map) : _map(&map) {}

        Map& GetMap()
        { return *this->_map; }
    };

}

#endif /* !__SERVER_PLAYER_HPP__ */
