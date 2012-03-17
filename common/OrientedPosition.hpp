#ifndef __COMMON_ORIENTEDPOSITION_HPP__
#define __COMMON_ORIENTEDPOSITION_HPP__

#include "common/Position.hpp"

namespace Common {

    struct OrientedPosition
    {
    public:
        Common::Position position;
        float const theta;
        float const phi;
        Tools::Vector3f const direction;

    public:
        OrientedPosition() :
            theta(0),
            phi(std::atan2(0.0f, -1.0f) / 2) // pi / 2
        {
            this->SetAngles(theta, phi);
        }

        OrientedPosition(Common::Position const& pos) :
            position(pos),
            theta(0),
            phi(std::atan2(0.0f, -1.0f) / 2) // pi / 2
        {
            this->SetAngles(theta, phi);
        }

        OrientedPosition(Common::Position const& pos, float theta, float phi) :
            position(pos), theta(theta), phi(phi)
        {
            this->SetAngles(theta, phi);
        }

        OrientedPosition& operator = (Common::OrientedPosition const& right)
        {
            const_cast<float&>(this->theta) = right.theta;
            const_cast<float&>(this->phi) = right.phi;
            const_cast<Tools::Vector3f&>(this->direction) = right.direction;
            return *this;
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

