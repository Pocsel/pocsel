#include "server/game/engine/PositionalEntity.hpp"

#include "common/physics/BodyCluster.hpp"

#include "bullet/bullet-all.hpp"

namespace Server { namespace Game { namespace Engine {

    PositionalEntity::PositionalEntity(
            Common::Physics::World& world,
            Tools::Lua::Interpreter& interpreter,
            Uint32 id,
            EntityType const& type,
            Common::Position const& pos) :
        Entity(interpreter, id, type),
        _bodyCluster(0)
    {
        this->_bodyCluster = new Common::Physics::BodyCluster(world, pos);
    }

    PositionalEntity::~PositionalEntity()
    {
        Tools::Delete(this->_bodyCluster);
    }

    void PositionalEntity::UpdatePhysics()
    {
        btRigidBody const& btBody = this->_bodyCluster->GetBody();
        Common::Physics::Node& physics = this->_physics;

        btTransform wt;
        btBody.getMotionState()->getWorldTransform(wt);

        btVector3 wpos = wt.getOrigin();
        physics.position = Common::Position(wpos.x(), wpos.y(), wpos.z());

        btQuaternion const& wrot = wt.getRotation();
        physics.orientation = glm::quat((float)wrot.w(), (float)wrot.x(), (float)wrot.y(), (float)wrot.z());

        btVector3 const& btVel = btBody.getLinearVelocity();
        physics.velocity = glm::vec3(btVel.x(), btVel.y(), btVel.z());

        btVector3 const& av = btBody.getAngularVelocity();
        physics.angularVelocity = glm::vec3(av.x(), av.y(), av.z());
    }

}}}
