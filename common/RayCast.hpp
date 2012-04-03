#ifndef __COMMON_RAYCAST_HPP__
#define __COMMON_RAYCAST_HPP__

#include "common/Position.hpp"

#include "tools/Vector3.hpp"

namespace Common {
    struct OrientedPosition;
    struct CubePosition;
}

namespace Common {

    struct CastChunk
    {
    public:
        enum CastType
        {
            Cube,
            Sphere
        };

    public:
        BaseChunk::IdType const id;
        bool const full;
    private:
        std::vector<BaseChunk::CoordsType> (CastChunk::*_getCubes)();
        Position _origin;
        float _distance;

    public:
        CastChunk(BaseChunk::IdType id) : id(id), full(true), _getCubes(0)
        {
        }
        CastChunk(BaseChunk::IdType id, CastType type, Position const& origin, float distance) : id(id), full(false), _origin(origin), _distance(distance)
        {
            switch (type)
            {
            case Cube:
                _getCubes = &CastChunk::_GetCubesCubeCast;
                break;
            case Sphere:
                _getCubes = &CastChunk::_GetCubesSphereCast;
                break;
            }
        }
        std::vector<BaseChunk::CoordsType> GetCubes()
        {
            assert(this->full == false && "Cannot get cubes on a full CastChunk");
            return (this->*this->_getCubes)();
        }
    private:
        std::vector<BaseChunk::CoordsType> _GetCubesCubeCast();
        std::vector<BaseChunk::CoordsType> _GetCubesSphereCast();
    };

    class RayCast
    {
    public:
        typedef NChunk<3> BigChunk;
    public:
        static std::vector<Common::CubePosition> Ray(Common::OrientedPosition const& pos, float distance);
        static std::vector<CastChunk*> SphereArea(Common::Position const& pos, float distance);
        static std::vector<CastChunk*> CubeArea(Common::Position const& pos, float distance);
    };

}

#endif
