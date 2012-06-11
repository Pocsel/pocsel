#ifndef __SERVER_NETWORK_BODYSERIALIZER_HPP__
#define __SERVER_NETWORK_BODYSERIALIZER_HPP__

#include "tools/ByteArray.hpp"

#include "server/game/engine/Body.hpp"
#include "tools/physics/BodySerializer.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Server::Game::Engine::Body > :
        public Tools::Physics::BodySerializer<Server::Game::Engine::Body>
        {
        };

}

#endif
