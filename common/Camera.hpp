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

        glm::mat4 GetViewMatrix() const
        {
            return glm::lookAt(glm::vec3(0.0f), this->direction, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glm::dmat4 GetAbsoluteViewProjectionMatrix() const
        {
            return glm::dmat4x4(this->projection) * glm::lookAt(this->position, this->position + glm::dvec3(this->direction), glm::dvec3(0.0, 1.0, 0.0));
        }
    };

}

#endif
