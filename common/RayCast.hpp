#ifndef __COMMON_RAYCAST_HPP__
#define __COMMON_RAYCAST_HPP__

#include "common/BaseChunk.hpp"

#include "tools/Vector3.hpp"

namespace Common {

    struct Camera;
    struct CubePosition;
    struct Position;

    class RayCast
    {
    public:
        static std::vector<Common::CubePosition> Ray(Common::Camera const& cam, float distance);
        static std::vector<Common::CubePosition> SphereArea(Common::Position const& pos, float distance);
        static std::vector<Common::CubePosition> CubeArea(Common::Position const& pos, float distance);
    };

}

#endif
