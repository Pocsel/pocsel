#ifndef __COMMON_MOVINGCAMERA_HPP__
#define __COMMON_MOVINGCAMERA_HPP__

#include "common/Camera.hpp"

namespace Common {

    struct MovingCamera
    {
    public:
        Common::Camera cam;
        Tools::Vector3f movement;

    public:
        MovingCamera() :
            cam(),
            movement()
        {
        }

        MovingCamera(Common::Position const& pos) :
            cam(pos),
            movement()
        {
        }

        MovingCamera(Common::Position const& pos, float theta, float phi) :
            cam(pos, theta, phi),
            movement()
        {
        }

        MovingCamera(Common::Camera const& cam) :
            cam(cam),
            movement()
        {
        }

        void Move(float time)
        {
            cam.position += movement * time;
        }

    };

}

#endif

