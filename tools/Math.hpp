#ifndef __TOOLS_MATH_HPP__
#define __TOOLS_MATH_HPP__

namespace Tools { namespace Math {

        // Computes the W component of the quaternion based on the X, Y, and Z components.
        // This method assumes the quaternion is of unit length.
        inline void ComputeQuatW(glm::quat& quat)
        {
            float t = 1.0f - (quat.x * quat.x) - (quat.y * quat.y) - (quat.z * quat.z);
            quat.w = t < 0.0f ? 0.0f : -std::sqrt(t);
        }

        static const double Pi = 3.1415926535897932384626433832795;
        static const float PiFloat = 3.1415926535897932384626433832795f;

}}

#endif
