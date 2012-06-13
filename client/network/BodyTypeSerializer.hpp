#ifndef __CLIENT_NETWORK_BODYTYPESERIALIZER_HPP__
#define __CLIENT_NETWORK_BODYTYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "common/physics/BodyTypeSerializer.hpp"
#include "client/game/engine/BodyType.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Client::Game::Engine::BodyType > :
        public Common::Physics::BodyTypeSerializer<Client::Game::Engine::BodyType>
        {
        };

}

#endif
