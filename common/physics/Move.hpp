#ifndef __COMMON_PHYSICS_MOVE_HPP__
#define __COMMON_PHYSICS_MOVE_HPP__

#include "common/physics/Node.hpp"
#include "common/physics/Vector.hpp"

namespace Common { namespace Physics {

//    template<typename T> void MoveVector(Common::Physics::Vector<T>& vector, T deltaTime)
//    {
//        if (vector.a.length() || vector.v.length())
//        {
//            glm::detail::tvec3<T> v0 = vector.v;
//            vector.v += deltaTime * vector.a;
//
//            vector.r += (vector.v + v0) * ((T)0.5 * deltaTime);
//        }
//    }
//
//    void MoveNode(Common::Physics::Node& physicsNode, double deltaTime)
//    {
//        MoveVector(physicsNode.position, deltaTime);
//        MoveVector(physicsNode.yawPitchRoll, (float)deltaTime);
//        MoveVector(physicsNode.scale, (float)deltaTime);
//    }

}}

#endif
