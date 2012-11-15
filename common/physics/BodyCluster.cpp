#include "common/physics/BodyCluster.hpp"
#include "common/physics/Body.hpp"
#include "common/physics/BodyType.hpp"
#include "common/physics/Utils.hpp"
#include "common/physics/World.hpp"

//#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    BodyCluster::BodyCluster(World& world, Node const& pos) :
        _world(world),
        _motionState(0),
        _body(0),
        _userData(0),
        _acceleration(0, 0, 0),
        _accelerationIsLocal(false)
        //_curMass(1)
    {
        btScalar mass(1);
        btVector3 localInertia(1, 1, 1);

        static std::unique_ptr<btCollisionShape> emptyShape(new btEmptyShape()); //btSphereShape(1);//btEmptyShape();
        //emptyShape->calculateLocalInertia(mass, localInertia);
        //std::cout << "INERTIAAIAIAIAI " << localInertia.x() << ", " << localInertia.y() << ", " << localInertia.z() << "\n";

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(pos.position.x, pos.position.y, pos.position.z));
        startTransform.setRotation(btQuaternion(pos.orientation.x,
                    pos.orientation.y,
                    pos.orientation.z,
                    pos.orientation.w));

        this->_motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, _motionState, emptyShape.get(), localInertia);
        this->_body = new btRigidBody(rbInfo);
        this->_body->setActivationState(DISABLE_DEACTIVATION);

        this->_body->setUserPointer(this);

        this->_body->setLinearFactor(btVector3(1,1,1));
        this->_body->setAngularFactor(btVector3(1,1,1));

        this->_world.AddBodyCluster(this);
        this->_world.GetBtWorld().addRigidBody(_body);

        this->_body->setGravity(btVector3(0, 0, 0));
    }

    BodyCluster::~BodyCluster()
    {
        assert(this->_constraints.empty() && "il faut vider les constraints avant de delete le cluster");

        this->_world.RemoveBodyCluster(this);
        this->_world.GetBtWorld().removeRigidBody(this->_body);

        Tools::Delete(_body);
        Tools::Delete(_motionState);
    }

    void BodyCluster::Tick()
    {
        //std::cout << "tick\n";
        if (this->_acceleration == btVector3(0, 0, 0))
            return this->_ClearTickAccel();
        //std::cout << "accel exists\n";

        // limitation de la vitesse
        btVector3 velocity = this->_body->getLinearVelocity();
        //btQuaternion velocityQ;
        //if (velocity.length() != 0)
        //    velocityQ = btQuaternion(velocity, 0);
        btQuaternion directionQ = this->_body->getCenterOfMassTransform().getRotation();
        //directionQ.normalize();
        //btVector3 direction = directionQ.getAxis();//(directionQ.x(), directionQ.y(), directionQ.z());
        //direction = direction.normalize();


        btVector3 accel = this->_acceleration;
        if (this->_accelerationIsLocal)
            accel = quatRotate(directionQ, accel);

        btVector3 targetDirection = accel;
        targetDirection.normalize();

        //btVector3 direction = quatRotate(directionQ, btVector3(1, 0, 0));

        btScalar speed = velocity.dot(targetDirection);

        //std::cout << "speed " << speed << " maxspeed " << this->_maxSpeed << "\n";
        if (speed >= this->_maxSpeed)
            return this->_ClearTickAccel();
        //std::cout << "applying force and shit\n";


        //this->_body->applyCentralImpulse(accel);
        this->_body->setGravity(this->_world.GetGravity() + accel);
        for (auto body: _constraints)
            body->_ApplyAccel(accel);

        //std::cout << "0speed= " << speed << " | ";
        ////btScalar speedQ = velocityQ.dot(directionQ);
        ////std::cout << "Qspeed= " << speedQ << "\n";

        //std::cout << "direction= " << direction.x() << " " << direction.y() << " " << direction.z() << " | " <<
        //    "velocity= " << velocity.x() << " " << velocity.y() << " " << velocity.z() << "\n";

        //this->_body->setLinearVelocity(direction * 10);

        //if (speed > 3)
        //{
        //    velocity.setX(3);
        //    this->_body->setLinearVelocity(velocity);
        //}
        /*
        btScalar speed = velocity.length();
        if (speed > 3) {
            velocity *= 3 / speed;
            this->_body->setLinearVelocity(velocity);
        }
        */
        //this->_body->setAngularVelocity(btVector3(0, 360, 0));
    }

    void BodyCluster::AddConstraint(Body* body)
    {
        this->_constraints.push_back(body);

        //{ // pecho la masse totale
        //    for (auto const& shape: body->GetType().GetShapes())
        //        this->_curMass += shape.mass;

        //    this->_body->setMassProps(this->_curMass, btVector3(1, 1, 1));
        //}
    }

    void BodyCluster::RemoveConstraint(Body* body)
    {
        for (auto it = this->_constraints.begin(), ite = this->_constraints.end(); it != ite; ++it)
        {
            if (*it == body)
            {
                this->_constraints.erase(it);

                //{ // pecho la masse totale
                //    for (auto const& shape: body->GetType().GetShapes())
                //        this->_curMass -= shape.mass;

                //    this->_body->setMassProps(this->_curMass, btVector3(1, 1, 1));
                //}

                return;
            }
        }
        assert(true && "this body has not been found");
    }

    void BodyCluster::SetPhysics(std::vector<Node> const& physics)
    {
        for (auto& Body: this->_constraints)
        {
            Body->_RemoveFromWorld();
        }

        btRigidBody& btBody = *this->_body;

        this->_world.GetBtWorld().removeRigidBody(&btBody);

        Utils::PhysicsNodeToBtBody(physics[0], btBody);

        if (physics[0].accelerationIsLocal)
            this->SetLocalAccel(
                    btVector3(physics[0].acceleration.x, physics[0].acceleration.y, physics[0].acceleration.z),
                    physics[0].maxSpeed);
        else
            this->SetAccel(
                    btVector3(physics[0].acceleration.x, physics[0].acceleration.y, physics[0].acceleration.z),
                    physics[0].maxSpeed);

        //btTransform wt;
        //wt.setOrigin(btVector3(
        //            physics.position.x,
        //            physics.position.y,
        //            physics.position.z
        //            ));
        //wt.setRotation(btQuaternion(
        //            physics.orientation.x,
        //            physics.orientation.y,
        //            physics.orientation.z,
        //            physics.orientation.w
        //            ));

        //btBody.setLinearVelocity(btVector3(
        //            physics.velocity.x,
        //            physics.velocity.y,
        //            physics.velocity.z
        //            ));
        //btBody.setAngularVelocity(btVector3(
        //            physics.angularVelocity.x,
        //            physics.angularVelocity.y,
        //            physics.angularVelocity.z
        //            ));

        ////std::cout 
        ////           <<  " POS "  << physics.position.x
        ////           <<  " , "  << physics.position.y
        ////           <<  " , "  << physics.position.z
        ////           <<  " OR "  << physics.orientation.x
        ////           <<  " , "  << physics.orientation.y
        ////           <<  " , "  << physics.orientation.z
        ////           <<  " , "  << physics.orientation.w

        ////           <<  " VEL "  << physics.velocity.x
        ////           <<  " , "  << physics.velocity.y
        ////           <<  " , "  << physics.velocity.z
        ////           <<  " AVEL "  << physics.angularVelocity.x
        ////           <<  " , "  << physics.angularVelocity.y
        ////           <<  " , "  << physics.angularVelocity.z << "\n";


        //btBody.clearForces();
        //btBody.getMotionState()->setWorldTransform(wt);
        //btBody.setCenterOfMassTransform(wt);

        auto physicsIt = physics.begin();
        auto physicsEnd = physics.end();

        for (auto body: this->_constraints)
        {
            if (physicsIt == physicsEnd)
            {
                std::cout << "BLIP BLOP FAIL0\n";
                break;
            }
            //body->_UpdatePosition();
            for (auto& bodyNode: body->GetNodes())
            {
                ++physicsIt;
                if (physicsIt == physicsEnd)
                {
                    std::cout << "BLIP BLOP FAIL1\n";
                    break;
                }
                Utils::PhysicsNodeToBtBody(*physicsIt, *bodyNode.body);

                const_cast<btVector3&>(bodyNode.acceleration) = btVector3(physicsIt->acceleration.x, physicsIt->acceleration.y, physicsIt->acceleration.z);
                const_cast<double&>(bodyNode.maxSpeed) = physicsIt->maxSpeed;
                const_cast<bool&>(bodyNode.accelerationIsLocal) = physicsIt->accelerationIsLocal;
            }
        }

        this->_world.GetBtWorld().addRigidBody(&btBody);

        for (auto body: this->_constraints)
        {
            body->_PutBackInWorld();
        }
    }

    void BodyCluster::SetAccel(btVector3 const& accel, btScalar maxSpeed)
    {
        //std::cout << "accel: " << accel.x() << ", " << accel.y() << ", " << accel.z() << "\n";
        this->_acceleration = accel;
        this->_maxSpeed = maxSpeed;
        this->_accelerationIsLocal = false;
    }

    void BodyCluster::SetLocalAccel(btVector3 const& accel, btScalar maxSpeed)
    {
        //std::cout << "local accel: " << accel.x() << ", " << accel.y() << ", " << accel.z() << "\n";
        this->_acceleration = accel;
        this->_maxSpeed = maxSpeed;
        this->_accelerationIsLocal = true;
    }

    std::vector<Common::Physics::Node> BodyCluster::GetClusterPhysics() const
    {
        std::vector<Common::Physics::Node> physics;

        // noeud maitre
        {
            physics.emplace_back(Utils::BtBodyToPhysicsNode(*this->_body));
            Common::Physics::Node& node = physics.back();
            btVector3 const& accel = this->_acceleration;
            node.acceleration = glm::dvec3(accel.x(), accel.y(), accel.z());
            if (accel != btVector3(0, 0, 0))
            {
                node.accelerationIsLocal = this->_accelerationIsLocal;
                node.maxSpeed = this->_maxSpeed;
            }
        }

        for (Body* body: this->_constraints)
        {
            for (auto& bodyNode: body->GetNodes())
            {
                physics.emplace_back(Utils::BtBodyToPhysicsNode(*bodyNode.body));

                auto& node = physics.back();
                btVector3 const& accel = bodyNode.acceleration;
                node.acceleration = glm::dvec3(accel.x(), accel.y(), accel.z());
                if (accel != btVector3(0, 0, 0))
                {
                    node.accelerationIsLocal = bodyNode.accelerationIsLocal;
                    node.maxSpeed = bodyNode.maxSpeed;
                }
            }
        }

        return physics;
    }

    void BodyCluster::SetUserData(void* userData)
    {
        this->_userData = userData;
        this->_body->setUserPointer(userData);
    }

    void BodyCluster::Dump() const
    {
        std::cout << "BodyCluster::Dump()\n";

        btTransform tr;
        this->_motionState->getWorldTransform(tr);
        std::cout << "    " <<
            tr.getOrigin().x() << ", " <<
            tr.getOrigin().y() << ", " <<
            tr.getOrigin().z() << "\n";

        for (auto& body: this->_constraints)
            body->Dump();
    }

    void BodyCluster::_ClearTickAccel()
    {
        this->_body->setGravity(this->_world.GetGravity());
        for (auto body: _constraints)
            body->_ApplyAccel(btVector3(0, 0, 0));
    }

}}
