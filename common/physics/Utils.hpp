#ifndef __COMMON_PHYSICS_UTILS_HPP__
#define __COMMON_PHYSICS_UTILS_HPP__

#include "common/physics/Node.hpp"
#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    struct Utils {

        static Node BtBodyToPhysicsNode(btRigidBody const& body)
        {
            Node node;

            btTransform wt;
            body.getMotionState()->getWorldTransform(wt);

            btVector3 wpos = wt.getOrigin();
            node.position = Common::Position(wpos.x(), wpos.y(), wpos.z());
            btQuaternion const& wrot = wt.getRotation();
            node.orientation = glm::dquat((float)wrot.w(), (float)wrot.x(), (float)wrot.y(), (float)wrot.z());

            btVector3 const& btVel = body.getLinearVelocity();
            node.velocity = glm::vec3(btVel.x(), btVel.y(), btVel.z());
            btVector3 const& av = body.getAngularVelocity();
            node.angularVelocity = glm::vec3(av.x(), av.y(), av.z());

            return node;
        }

        static void PhysicsNodeToBtBody(Node const& physics, btRigidBody& body)
        {
            btTransform wt;
            wt.setOrigin(btVector3(
                        physics.position.x,
                        physics.position.y,
                        physics.position.z
                        ));
            wt.setRotation(btQuaternion(
                        physics.orientation.x,
                        physics.orientation.y,
                        physics.orientation.z,
                        physics.orientation.w
                        ));

            body.setLinearVelocity(btVector3(
                        physics.velocity.x,
                        physics.velocity.y,
                        physics.velocity.z
                        ));
            body.setAngularVelocity(btVector3(
                        physics.angularVelocity.x,
                        physics.angularVelocity.y,
                        physics.angularVelocity.z
                        ));

            body.clearForces();
            body.getMotionState()->setWorldTransform(wt);
            body.setCenterOfMassTransform(wt);
        }

    };

}}

#endif
