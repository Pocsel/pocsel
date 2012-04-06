#ifndef __TOOLS_MATH_HPP__
#define __TOOLS_MATH_HPP__

namespace Tools {

    class Math
    {
    public:
        // Computes the W component of the quaternion based on the X, Y, and Z components.
        // This method assumes the quaternion is of unit length.
        static void ComputeQuatW( glm::quat& quat )
        {
            float t = 1.0f - ( quat.x * quat.x ) - ( quat.y * quat.y ) - ( quat.z * quat.z );
            if ( t < 0.0f )
            {
                quat.w = 0.0f;
            }
            else
            {
                quat.w = -sqrtf(t);
            }
        }

    };

}

#endif
