#include "server/game/engine/PhysicsManager.hpp"
#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/DoodadManager.hpp"

#include "common/physics/Node.hpp"
#include "common/physics/Vector.hpp"
#include "common/physics/World.hpp"
#include "common/physics/BodyCluster.hpp"

#include "bullet/bullet-all.hpp"

#include "tools/lua/Function.hpp"

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

                        //if (body0->getUserPointer())
                        //    pm->_newCollidingEntities.insert((PositionalEntity*)body0->getUserPointer());
                        //if (body1->getUserPointer())
                        //    pm->_newCollidingEntities.insert((PositionalEntity*)body1->getUserPointer());


                        if (body0->getUserPointer() && body1->getUserPointer() &&
                                body0->getUserPointer() != body1->getUserPointer())
                        {
                            PositionalEntity* entity0 = (PositionalEntity*)body0->getUserPointer();
                            PositionalEntity* entity1 = (PositionalEntity*)body1->getUserPointer();


                            pm->_newCollidingEntities.insert(entity0);
                            pm->_newCollidingEntities.insert(entity1);

                            //entity0->SetIsDirty(true);
                            //entity1->SetIsDirty(true);

                            //Uint32 id0 = entity0->GetSelf()["id"].To<Uint32>();
                            //Uint32 id1 = entity1->GetSelf()["id"].To<Uint32>();
                            //pm->_engine.GetDoodadManager().EntityHasMoved(id0);
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

        static Uint64 totalTime = 0;
        static Uint64 lastTime = 0;

        totalTime += deltaTime;
        if (totalTime > lastTime)
        {
            while (lastTime < totalTime)
                lastTime += 1000000;
            for (auto it = _entities.begin(), ite = _entities.end(); it != ite; ++it)
            {
                it->second->SetIsDirty(true);
            }
        }




        if (_entities.size())
            for (auto it = _entities.begin(); it == _entities.begin(); ++it)
            {
                PositionalEntity* entity = it->second;
                btRigidBody& body = entity->GetBodyCluster().GetBody();

                std::cout << 
                    "vel:     " <<
                    body.getLinearVelocity().x() << ", " <<
                    body.getLinearVelocity().y() << ", " <<
                    body.getLinearVelocity().z() << "\n";

                std::cout <<
                    "angvel:  " <<
                    body.getAngularVelocity().x() << ", " <<
                    body.getAngularVelocity().y() << ", " <<
                    body.getAngularVelocity().z() << "\n";

                std::cout <<
                    "othershit: " <<
                    body.getVelocityInLocalPoint(btVector3(3, 3, 3)).x() << ", " <<
                    body.getVelocityInLocalPoint(btVector3(3, 3, 3)).y() << ", " <<
                    body.getVelocityInLocalPoint(btVector3(3, 3, 3)).z() << "\n";
            }

        this->_world->Tick(deltaTime);

        this->_UpdateCollidingEntities();

        for (auto it = this->_entities.begin(), ite = this->_entities.end(); it != ite; ++it)
        {
            if (!it->second)
                continue;

            PositionalEntity& entity = *it->second;
            entity.UpdatePhysics();
        }
    }

    void PhysicsManager::_UpdateCollidingEntities()
    {
        for (auto it = this->_newCollidingEntities.begin(), ite = this->_newCollidingEntities.end(); it != ite; ++it)
        {
            (*it)->SetIsDirty(true);
        }
        for (auto it = this->_collidingEntities.begin(), ite = this->_collidingEntities.end(); it != ite; ++it)
        {
            if (!this->_newCollidingEntities.count(*it))
            {
                (*it)->SetIsDirty(true);
            }
        }
        this->_collidingEntities = this->_newCollidingEntities;
        this->_newCollidingEntities.clear();
    }

}}}
