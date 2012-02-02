#ifndef __CLIENT_CUBETYPEMANAGER_HPP__
#define __CLIENT_CUBETYPEMANAGER_HPP__

#include "Chunk.hpp"
#include "common/CubeType.hpp"

namespace Tools { namespace Gui {
    class EventManager;
} }

namespace Common {
    class Packet;
}

namespace Client {

    class Network;
    class PacketDispatcher;

    class CubeTypeManager
    {
        private:
            Network& _network;
            Tools::Gui::EventManager& _eventManager;
            Chunk::CubeType _nbCubeTypes;
            Chunk::CubeType _curAskedType;
            std::vector<Common::CubeType> _cubeTypes;


        public:
            CubeTypeManager(Tools::Gui::EventManager& eventManager,
                            Network& network,
                            PacketDispatcher& packetDispatcher);

            void Load(Chunk::CubeType nbCubeTypes);

            std::vector<Common::CubeType> const& GetCubeTypes() const {return this->_cubeTypes;}

        private:
            void _HandleCubeType(Common::Packet const& p);
            void _AskOneType();
    };

}

#endif
