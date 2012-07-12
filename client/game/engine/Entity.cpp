#include "client/game/engine/Entity.hpp"

#include "common/physics/BodyCluster.hpp"

namespace Client { namespace Game { namespace Engine {

    Entity::Entity(Common::Physics::World& world, Uint32 id, Common::Physics::Node const& pos) :
        _id(id),
        _bodyCluster(0),
        _updateFlag(0.0f)
    {
        this->_bodyCluster = new Common::Physics::BodyCluster(world, pos);
    }

    Entity::~Entity()
    {
        Tools::Delete(this->_bodyCluster);
    }

}}}
