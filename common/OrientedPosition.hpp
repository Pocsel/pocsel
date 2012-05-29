#ifndef __COMMON_ORIENTEDPOSITION_HPP__
#define __COMMON_ORIENTEDPOSITION_HPP__

#include "tools/Math.hpp"

#include "common/Position.hpp"


namespace Common {

    struct OrientedPosition
    {
    public:
        Common::Position position;
        float const theta;
        float const phi;
        glm::fvec3 const direction;

    public:
        OrientedPosition() :
            theta(0),
            phi(Tools::Math::Pi / 2)
        {
            this->SetAngles(theta, phi);
        }

        OrientedPosition(Common::Position const& pos) :
            position(pos),
            theta(0),
            phi(Tools::Math::Pi / 2)
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
            this->position = right.position;
            const_cast<float&>(this->theta) = right.theta;
            const_cast<float&>(this->phi) = right.phi;
            const_cast<glm::fvec3&>(this->direction) = right.direction;
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

            glm::fvec3& dir = const_cast<glm::fvec3&>(this->direction);
            dir.x = sinphi * costheta;
            dir.y = cosphi;
            dir.z = sinphi * sintheta;
            dir = glm::normalize(dir);
        }

        void Rotate(glm::fvec2 const& delta)
        {
            this->Rotate(delta.x, delta.y);
        }

        void Rotate(float dtheta, float dphi)
        {
            float& t = const_cast<float&>(this->theta);
            t += dtheta;
            if (t < 0 || t >= 2 * Tools::Math::Pi)
                t = std::fmod(t, 2 * Tools::Math::Pi);

            float& p = const_cast<float&>(this->phi);
            p += dphi;
            if (p < 0)
                p = 0.001f;
            else if (p > Tools::Math::Pi)
                p = Tools::Math::Pi - 0.001f;

            this->SetAngles(t, p);
        }
    };

}

#endif

