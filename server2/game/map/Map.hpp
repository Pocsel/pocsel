#ifndef __SERVER_GAME_MAP_MAP_HPP__
#define __SERVER_GAME_MAP_MAP_HPP__

#include "server2/game/map/Conf.hpp"

namespace Server { namespace Game { namespace Map {

    struct Conf;

    class Map :
        private boost::noncopyable
    {
    private:
        Conf _conf;

    public:
        Map(Conf const& conf);
        ~Map();
    };

}}}

#endif
