#ifndef __COMMON_CAMERA_HPP__
#define __COMMON_CAMERA_HPP__

#include "common/OrientedPosition.hpp"
#include "tools/Frustum.hpp"
#include "tools/Matrix4.hpp"

namespace Common {

    struct Camera :
        public OrientedPosition
    {
    public:
        Tools::Matrix4<float> projection;

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

        Tools::Matrix4<float> GetViewMatrix() const
        {
            return Tools::Matrix4<float>::CreateLookAt(Tools::Vector3f(0), this->direction, Tools::Vector3f(0, 1, 0));
        }
    };

}

#endif
