#include "server/game/engine/PhysicsManager.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/DoodadManager.hpp"

#include "common/physics/Node.hpp"
#include "common/physics/Vector.hpp"
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
                        // XXX
                        btRigidBody* body0 = (btRigidBody*)contactManifold->getBody0();
                        btRigidBody* body1 = (btRigidBody*)contactManifold->getBody1();

                        if (body0->getUserPointer() && body1->getUserPointer() &&
                                body0->getUserPointer() != body1->getUserPointer())
                        {
                            PositionalEntity* entity0 = (PositionalEntity*)body0->getUserPointer();
                            entity0->SetIsDirty(true);
                            //Uint32 id0 = entity0->GetSelf()["id"].To<Uint32>();

                            //pm->_engine.GetDoodadManager().EntityHasMoved(id0);

                            PositionalEntity* entity1 = (PositionalEntity*)body1->getUserPointer();
                            entity1->SetIsDirty(true);
                            //Uint32 id1 = entity1->GetSelf()["id"].To<Uint32>();

                            //pm->_engine.GetDoodadManager().EntityHasMoved(id1);
                        }

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
//        for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
//        {
//            if (!this->_bodiesInWorld.count(it->first))
//            {
//                //this->_world->GetBtWorld().addRigidBody(&it->second->GetBtBody());
//                this->_bodiesInWorld.insert(it->first);
//                //this->_entityBodies.insert(&it->second->GetBtBody());
//            }
//        }

        this->_world->Tick(deltaTime);

        for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        {
            if (!it->second)
                continue;

            PositionalEntity& entity = *it->second;
            entity.UpdatePhysics();
        }
    }

}}}
