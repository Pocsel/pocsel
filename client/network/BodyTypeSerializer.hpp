#ifndef __CLIENT_NETWORK_BODYTYPESERIALIZER_HPP__
#define __CLIENT_NETWORK_BODYTYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/StdVectorSerializer.hpp"

#include "client/game/engine/BodyType.hpp"
#include "tools/physics/BodyTypeSerializer.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< Client::Game::Engine::BodyType > :
        public Tools::Physics::BodyTypeSerializer<Client::Game::Engine::BodyType>
        {
        };

}

#endif
