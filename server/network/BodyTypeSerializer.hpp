#ifndef __SERVER_NETWORK_BODYTYPESERIALIZER_HPP__
#define __SERVER_NETWORK_BODYTYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/StdVectorSerializer.hpp"

#include "server/game/engine/BodyType.hpp"
#include "tools/physics/BodyTypeSerializer.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Server::Game::Engine::BodyType > :
        public Tools::Physics::BodyTypeSerializer<Server::Game::Engine::BodyType>
        {
        };

}

#endif
