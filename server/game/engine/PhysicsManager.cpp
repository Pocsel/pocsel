#include "server/game/engine/PhysicsManager.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/DoodadManager.hpp"

#include "common/physics/Node.hpp"
#include "common/physics/Vector.hpp"
#include "common/physics/Move.hpp"
#include "common/physics/World.hpp"

#include "bullet/bullet-all.hpp"

namespace Server { namespace Game { namespace Engine {

    struct _cb
    {
        static void _TickCallback(btDynamicsWorld* world, btScalar timeStep)
        {
            PhysicsManager* pm = (PhysicsManager*)world->getWorldUserInfo();

            int numManifolds = world->getDispatcher()->getNumManifolds();
            for (int i=0;i<numManifolds;i++)
            {
                btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
                //btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
                //btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());

                int numContacts = contactManifold->getNumContacts();

                for (int j=0;j<numContacts;j++)
                {
                    btManifoldPoint& pt = contactManifold->getContactPoint(j);
                    if (pt.getDistance()<0.f)
                    {
                        btRigidBody const* body0 = (btRigidBody const*)contactManifold->getBody0();
                        PositionalEntity const* entity0 = (PositionalEntity const*)body0->getUserPointer();
                        Uint32 id0 = entity0->GetSelf()["id"].To<Uint32>();

                        pm->_engine.GetDoodadManager().EntityHasMoved(id0);

                        btRigidBody const* body1 = (btRigidBody const*)contactManifold->getBody1();
                        PositionalEntity const* entity1 = (PositionalEntity const*)body1->getUserPointer();
                        Uint32 id1 = entity1->GetSelf()["id"].To<Uint32>();

                        pm->_engine.GetDoodadManager().EntityHasMoved(id1);

                        break;
                    }
                }

                //for (int j=0;j<numContacts;j++)
                //{
                //    btManifoldPoint& pt = contactManifold->getContactPoint(j);
                //    if (pt.getDistance()<0.f)
                //    {
                //        const btVector3& ptA = pt.getPositionWorldOnA();
                //        const btVector3& ptB = pt.getPositionWorldOnB();
                //        const btVector3& normalOnB = pt.m_normalWorldOnB;
                //    }
                //}
            }
        }
    };

    PhysicsManager::PhysicsManager(Engine& engine, std::map<Uint32, PositionalEntity*> const& entities) :
        _engine(engine),
        _entities(entities),
        _world(0)
    {
        this->_world = new Common::Physics::World();
        this->_world->GetBtWorld().setInternalTickCallback(_cb::_TickCallback, this);
    }

    PhysicsManager::~PhysicsManager()
    {
        Tools::Delete(this->_world);
    }

    void PhysicsManager::Tick(Uint64 deltaTime)
    {
        for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        {
            if (!this->_bodiesInWorld.count(it->first))
            {
                this->_world->GetBtWorld().addRigidBody(&it->second->GetBtBody());
                this->_bodiesInWorld.insert(it->first);
            }
        }

        double dt = deltaTime / (double)1000000.0;

        this->_world->Tick(deltaTime);

        for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        {
            if (!it->second)
                continue;

            PositionalEntity& entity = *it->second;
            btRigidBody const& btBody = entity.GetBtBody();
            btVector3 const& btPos = btBody.getCenterOfMassPosition();
            btQuaternion const& btAngl = btBody.getCenterOfMassTransform().getRotation();
            btVector3 const& btVel = btBody.getLinearVelocity();
            Common::Physics::Node& physics = entity.GetPhysics();

            btTransform wt;
            btBody.getMotionState()->getWorldTransform(wt);
            btVector3 wpos = wt.getOrigin();

            physics.position.x = wpos.x();//BtPos.x();
            physics.position.y = wpos.y();//BtPos.y();
            physics.position.z = wpos.z();//BtPos.z();

            //physics.acceleration.x = btPos.x();
            //physics.acceleration.y = btPos.y();
            //physics.acceleration.z = btPos.z();
            //physics.scale.x = btAngl.x();
            //physics.scale.y = btAngl.y();
            //physics.scale.z = btAngl.z();
            //physics.scaleVelocity.x = btAngl.w();

            physics.velocity.x = btVel.x();
            physics.velocity.y = btVel.y();
            physics.velocity.z = btVel.z();

            btQuaternion wrot = wt.getRotation();
            glm::quat glmRot(wrot.w(), wrot.x(), wrot.y(), wrot.z());
            physics.orientation = //glm::eulerAngles(glmRot);
            glmRot;
            btVector3 av = btBody.getAngularVelocity();
            physics.angularVelocity = glm::vec3(av.x(), av.y(), av.z());

            //btVector3 ae = btBody.getTurnVelocity();
            //physics.angularAcceleration = glm::vec3(ae.x(), ae.y(), ae.z());


            // std::cout << physics.velocity.x << ", ";
            // std::cout << physics.velocity.y << ", ";
            // std::cout << physics.velocity.z << "\n";

            // TODO j'en suis là

            // Common::Physics::MoveNode(entity.GetPhysics(), dt);
        }

        //for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        //{
        //    if (!it->second)
        //        continue;
        //    PositionalEntity& entity = *it->second;

        //    Common::Physics::MoveNode(entity.GetPhysics(), dt);
        //}
    }

}}}
