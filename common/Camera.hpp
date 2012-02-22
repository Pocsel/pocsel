#ifndef __COMMON_CAMERA_HPP__
#define __COMMON_CAMERA_HPP__

#include "common/Position.hpp"
#include "tools/Frustum.hpp"
#include "tools/Matrix4.hpp"
#include "tools/Vector3.hpp"

namespace Common {

    struct Camera
    {
    public:
        Common::Position position;
        float const theta;
        float const phi;
        Tools::Vector3f const direction;
        Tools::Matrix4<float> projection;

    public:
        Camera() :
            theta(0),
            phi(std::atan2(0.0f, -1.0f) / 2) // pi / 2
        {
            this->SetAngles(theta, phi);
        }

        Camera(Common::Position const& pos) :
            position(pos),
            theta(0),
            phi(std::atan2(0.0f, -1.0f) / 2) // pi / 2
        {
            this->SetAngles(theta, phi);
        }

        Camera(Common::Position const& pos, float theta, float phi) :
            position(pos), theta(theta), phi(phi)
        {
            this->SetAngles(theta, phi);
        }

        Tools::Matrix4<float> GetViewMatrix() const
        {
            return Tools::Matrix4<float>::CreateLookAt(Tools::Vector3f(0), this->direction, Tools::Vector3f(0, 1, 0));
        }

        Tools::Frustum GetFrustum() const
        {
            return Tools::Frustum(Tools::Matrix4<double>(this->GetViewMatrix() * this->projection));
        }

        void SetAngles(float newtheta, float newphi)
        {
            const_cast<float&>(this->theta) = newtheta;
            const_cast<float&>(this->phi) = newphi;

            // recalculate direction
            float cosphi = std::cos(this->phi);
            float sinphi = std::sin(this->phi);
            float costheta = std::cos(this->theta);
            float sintheta = std::sin(this->theta);

            Tools::Vector3f& dir = const_cast<Tools::Vector3f&>(this->direction);
            dir.x = sinphi * costheta;
            dir.y = cosphi;
            dir.z = sinphi * sintheta;
            dir.Normalize();
        }

        void Rotate(Tools::Vector2f const& delta)
        {
            this->Rotate(delta.x, delta.y);
        }

        void Rotate(float dtheta, float dphi)
        {
            float const pi = std::atan2(0.0f,-1.0f);

            float& t = const_cast<float&>(this->theta);
            t += dtheta;
            if (t < 0 || t >= 2 * pi)
                t = std::fmod(t, 2 * pi);


            float& p = const_cast<float&>(this->phi);
            p += dphi;
            if (p < 0)
                p = 0.001f;
            else if (p > pi)
                p = pi - 0.001f;

            this->SetAngles(t, p);
        }
    };

}

#endif
