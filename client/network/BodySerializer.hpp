#ifndef __SERVER_NETWORK_BODYSERIALIZER_HPP__
#define __SERVER_NETWORK_BODYSERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "common/physics/BodySerializer.hpp"
#include "client/game/engine/Body.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Client::Game::Engine::Body > :
        public Common::Physics::BodySerializer<Client::Game::Engine::Body>
        {
        };

}

#endif
