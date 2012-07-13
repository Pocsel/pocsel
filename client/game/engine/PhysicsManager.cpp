#include "client/game/engine/PhysicsManager.hpp"
#include "client/game/engine/Doodad.hpp"
#include "client/game/engine/Engine.hpp"
#include "client/game/engine/DoodadManager.hpp"

#include "common/physics/Node.hpp"
#include "common/physics/Vector.hpp"
#include "common/physics/World.hpp"

#include "bullet/bullet-all.hpp"

#include "tools/lua/Function.hpp"

namespace Client { namespace Game { namespace Engine {

    void PhysicsManager::AddBody(btRigidBody* body)
    {
        if (body)
            this->_world->GetBtWorld().addRigidBody(body);
    }

    void PhysicsManager::RemoveBody(btRigidBody* body)
    {
        if (body)
            this->_world->GetBtWorld().removeRigidBody(body);
    }

    PhysicsManager::PhysicsManager(Engine& engine, std::map<Uint32, Doodad*> const& doodads) :
        _engine(engine),
        _doodads(doodads),
        _world(0),
        _lastTime(0)
    {
        this->_world = new Common::Physics::World();
        //this->_world->GetBtWorld().setInternalTickCallback(_cb::_TickCallback, this);
    }

    PhysicsManager::~PhysicsManager()
    {
        Tools::Delete(this->_world);
    }

    void PhysicsManager::Tick(Uint64 totalTime)
    {
        for (auto it = this->_doodads.begin(), ite = this->_doodads.end(); it != ite; ++it)
        {
            if (!this->_bodiesInWorld.count(it->first))
            {
//                this->_world->GetBtWorld().addRigidBody(&it->second->GetBtBody());
                this->_bodiesInWorld.insert(it->first);
//                this->_doodadBodies.insert(&it->second->GetBtBody());
            }
        }

        Uint64 deltaTime = totalTime - this->_lastTime;

        this->_world->Tick(deltaTime);
        this->_lastTime = totalTime;

//        for (auto it = this->_doodads.begin(), ite = this->_doodads.end(); it != ite; ++it)
//        {
//            if (!it->second)
//                continue;
//
//            Doodad& entity = *it->second;
//            btRigidBody const& btBody = entity.GetBtBody();
//            Common::Physics::Node& physics = entity.GetPhysics();
//
//            btTransform wt;
//            btBody.getMotionState()->getWorldTransform(wt);
//
//            btVector3 wpos = wt.getOrigin();
//            physics.position = Common::Position(wpos.x(), wpos.y(), wpos.z());
//
//            btQuaternion const& wrot = wt.getRotation();
//            physics.orientation = glm::quat(wrot.w(), wrot.x(), wrot.y(), wrot.z());
//
//            btVector3 const& btVel = btBody.getLinearVelocity();
//            physics.velocity = glm::vec3(btVel.x(), btVel.y(), btVel.z());
//
//            btVector3 const& av = btBody.getAngularVelocity();
//            physics.angularVelocity = glm::vec3(av.x(), av.y(), av.z());
//        }
    }

}}}

