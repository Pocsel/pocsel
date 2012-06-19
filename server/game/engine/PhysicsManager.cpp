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

                if (numContacts > 0)
                {
                    btRigidBody const* body0 = (btRigidBody const*)contactManifold->getBody0();
                    PositionalEntity const* entity0 = (PositionalEntity const*)body0->getUserPointer();
                    Uint32 id0 = entity0->GetSelf()["id"].To<Uint32>();

                    pm->_engine.GetDoodadManager().EntityHasMoved(id0);

                    btRigidBody const* body1 = (btRigidBody const*)contactManifold->getBody1();
                    PositionalEntity const* entity1 = (PositionalEntity const*)body1->getUserPointer();
                    Uint32 id1 = entity1->GetSelf()["id"].To<Uint32>();

                    pm->_engine.GetDoodadManager().EntityHasMoved(id1);

                    std::cout << "id0= " << id0 << ", id1=" << id1 << "\n";
//                    Uint32 id = ((PositionalEntity*)contactManifold->)
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
            btVector3 const& btPos = entity.GetBtBody().getCenterOfMassPosition();
            btVector3 const& btVel = entity.GetBtBody().getLinearVelocity();
            Common::Physics::Node& physics = entity.GetPhysics();

            btTransform wt;
            entity.GetBtBody().getMotionState()->getWorldTransform(wt);
            btVector3 wpos = wt.getOrigin();

            physics.position.r.x = wpos.x();//BtPos.x();
            physics.position.r.y = wpos.y();//BtPos.y();
            physics.position.r.z = wpos.z();//BtPos.z();

            physics.position.v.x = btVel.x();
            physics.position.v.y = btVel.y();
            physics.position.v.z = btVel.z();


            // std::cout << physics.position.v.x << ", ";
            // std::cout << physics.position.v.y << ", ";
            // std::cout << physics.position.v.z << "\n";

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
