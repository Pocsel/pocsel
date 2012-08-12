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
            this->_bodyCluster->GetBody().getMotionState()->getWorldTransform(wt);
            btVector3 wpos = wt.getOrigin();

            physics.position.x = wpos.x();
            physics.position.y = wpos.y();
            physics.position.z = wpos.z();

            btQuaternion wrot = wt.getRotation();
            glm::quat glmRot((float)wrot.w(), (float)wrot.x(), (float)wrot.y(), (float)wrot.z());
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

    void Entity::UpdatePosition(Common::Physics::Node const& position)
    {
        btRigidBody& btBody = this->_bodyCluster->GetBody();

        /*
        {
            btTransform curWt;
            btBody.getMotionState()->getWorldTransform(curWt);
            std::cout << "Entity::UpdatePosition: " << "\n" <<
                std::setprecision(50) <<
                "curX = " << curWt.getOrigin().x() << "\n" <<
                "curY = " << curWt.getOrigin().y() << "\n" <<
                "curZ = " << curWt.getOrigin().z() << "\n" <<
                "newX = " << position.position.x << "\n" <<
                "newY = " << position.position.y << "\n" <<
                "newZ = " << position.position.z << "\n";
        }
        */


        btTransform wt;
        wt.setOrigin(btVector3(position.position.x,
                    position.position.y,
                    position.position.z));
        wt.setRotation(btQuaternion(
                    position.orientation.x,
                    position.orientation.y,
                    position.orientation.z,
                    position.orientation.w));

        this->_bodyCluster->GetWorld().GetBtWorld().removeRigidBody(&btBody);

        btBody.setLinearVelocity(
                btVector3(position.velocity.x,
                    position.velocity.y,
                    position.velocity.z));

        btBody.setAngularVelocity(
                btVector3(position.angularVelocity.x,
                    position.angularVelocity.y,
                    position.angularVelocity.z));

        btBody.clearForces();
        btBody.getMotionState()->setWorldTransform(wt);
        btBody.setCenterOfMassTransform(wt);

        this->_bodyCluster->GetWorld().GetBtWorld().addRigidBody(&btBody);
    }

}}}
