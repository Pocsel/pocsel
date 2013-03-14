#include "client/game/engine/Entity.hpp"

#include "common/physics/BodyCluster.hpp"
#include "common/physics/World.hpp"

#include "bullet/bullet-all.hpp"

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

    Common::Physics::Node const& Entity::GetPhysics() const
    {
        if (this->_updatedPhysics == false)
        {
            Common::Physics::Node& physics = this->_physics;

            btTransform wt;
            this->_bodyCluster->GetBtBody().getMotionState()->getWorldTransform(wt);
            btVector3 wpos = wt.getOrigin();

            physics.position.x = wpos.x();
            physics.position.y = wpos.y();
            physics.position.z = wpos.z();

            btQuaternion wrot = wt.getRotation();
            glm::dquat glmRot((float)wrot.w(), (float)wrot.x(), (float)wrot.y(), (float)wrot.z());
            physics.orientation = //glm::eulerAngles(glmRot);
                glmRot;

            this->_updatedPhysics = true;
        }
        return this->_physics;
    }

    void Entity::AddDoodad(Uint32 doodad)
    {
        this->_doodads.push_back(doodad);
    }

    void Entity::UpdatePhysics(std::vector<Common::Physics::Node> const& position)
    {
        this->_bodyCluster->SetPhysics(position);

        // TODO
        //if (position.accelerationIsLocal)
        //    this->_bodyCluster->SetLocalAccel(
        //            btVector3(position.acceleration.x, position.acceleration.y, position.acceleration.z),
        //            position.maxSpeed);
        //else
        //    this->_bodyCluster->SetAccel(
        //            btVector3(position.acceleration.x, position.acceleration.y, position.acceleration.z),
        //            position.maxSpeed);
    }

}}}
