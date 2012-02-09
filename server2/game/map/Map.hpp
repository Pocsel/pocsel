#ifndef __SERVER_GAME_MAP_MAP_HPP__
#define __SERVER_GAME_MAP_MAP_HPP__

#include "server2/game/map/Conf.hpp"

#include "tools/SimpleMessageQueue.hpp"

namespace Server { namespace Game { namespace Map {

    struct Conf;

    class Map :
        public Tools::SimpleMessageQueue,
        private boost::noncopyable
    {
    private:
        Conf _conf;

    public:
        Map(Conf const& conf);
        ~Map();

        void Start();
        void Stop();
    };

}}}

#endif
