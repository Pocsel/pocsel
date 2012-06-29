#ifndef __COMMON_CAMERA_HPP__
#define __COMMON_CAMERA_HPP__

#include "common/OrientedPosition.hpp"
#include "tools/Frustum.hpp"

namespace Common {

    struct Camera :
        public OrientedPosition
    {
    public:
        glm::detail::tmat4x4<float> projection;

    public:
        Camera() :
            OrientedPosition()
        {
        }

        Camera(Common::Position const& pos) :
            OrientedPosition(pos)
        {
        }

        Camera(Common::Position const& pos, float theta, float phi) :
            OrientedPosition(pos, theta, phi)
        {
        }

        glm::detail::tmat4x4<float> GetViewMatrix() const
        {
            return glm::lookAt<float>(glm::fvec3(0), this->direction, glm::fvec3(0, 1, 0));
        }
    };

}

#endif
