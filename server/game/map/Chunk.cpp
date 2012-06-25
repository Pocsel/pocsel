#include "server/game/map/Chunk.hpp"
#include "common/physics/Chunk.hpp"

#include "bullet/bullet-all.hpp"

namespace Server { namespace Game { namespace Map {

    Chunk::Chunk(IdType id) :
        Common::BaseChunk(id),
        _physicsChunk(0)
    {
    }

    Chunk::Chunk(CoordsType const& coords) :
        Common::BaseChunk(coords),
        _physicsChunk(0)
    {
    }

    Chunk::~Chunk()
    {
    }

    void Chunk::InitBody()
    {
        this->_physicsChunk = new Common::Physics::Chunk(*this);
    }

    btRigidBody* Chunk::GetBody()
    {
        return this->_physicsChunk->GetBody();
    }

}}}
