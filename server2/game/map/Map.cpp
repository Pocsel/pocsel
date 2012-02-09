#include "server2/game/map/Map.hpp"

namespace Server { namespace Game { namespace Map {

    Map::Map(Conf const& conf) :
        Tools::SimpleMessageQueue(1),
        _conf(conf)
    {
        Tools::debug << "Map::Map() -- " << this->_conf.name << "\n";
    }

    Map::~Map()
    {
        Tools::debug << "Map::~Map() -- " << this->_conf.name << "\n";
    }

}}}
