#ifndef __SERVER_NETWORK_BODYSERIALIZER_HPP__
#define __SERVER_NETWORK_BODYSERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "common/physics/BodySerializer.hpp"
#include "server/game/engine/Body.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Server::Game::Engine::Body > :
        public Common::Physics::BodySerializer<Server::Game::Engine::Body>
        {
        };

}

#endif
