#ifndef __COMMON_RAYCAST_HPP__
#define __COMMON_RAYCAST_HPP__

#include "common/BaseChunk.hpp"

#include "tools/Vector3.hpp"

namespace Common {

    struct Camera;
    struct CubePosition;

    class RayCast
    {
    public:
        static std::vector<Common::CubePosition> GetResult(Common::Camera const& cam, float distance);
    };

}

#endif
