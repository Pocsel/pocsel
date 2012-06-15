#include "common/physics/Body.hpp"
#include "common/physics/BodyType.hpp"

#include "bullet/bullet-all.hpp"


namespace Common { namespace Physics {

    Body::Body(BodyType const& bodyType) : //, Common::Physics::Node const& pos) :
        _type(bodyType),
        _nodes(bodyType.GetShapes().size())
        /*
        ,
        _btBody(0)
        */
    {
        /*
        static btCollisionShape* colShape = new btBoxShape(btVector3(1, 2, 1));

        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();

        btScalar mass(12);
        btVector3 localInertia(0, 0, 0);

        colShape->calculateLocalInertia(mass, localInertia);

        startTransform.setOrigin(btVector3(
                    btScalar(pos.position.r.x),
                    btScalar(pos.position.r.y),
                    btScalar(pos.position.r.z)));

        //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
        _btBody = new btRigidBody(rbInfo);
        */
    }

    Body::~Body()
    {
        /*
        Tools::Delete(this->_btBody);
        */
    }

}}
