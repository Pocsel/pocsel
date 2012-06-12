#ifndef __SERVER_NETWORK_BODYTYPESERIALIZER_HPP__
#define __SERVER_NETWORK_BODYTYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "common/physics/BodyTypeSerializer.hpp"
#include "server/game/engine/BodyType.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Server::Game::Engine::BodyType > :
        public Common::Physics::BodyTypeSerializer<Server::Game::Engine::BodyType>
        {
        };

}

#endif
