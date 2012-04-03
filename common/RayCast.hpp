#ifndef __COMMON_RAYCAST_HPP__
#define __COMMON_RAYCAST_HPP__

#include "common/Position.hpp"
#include "common/CubePosition.hpp"

#include "tools/Vector3.hpp"

namespace Common {
    struct OrientedPosition;
}

namespace Common {

    struct CastChunk
    {
    public:
        BaseChunk::IdType id;
    private:
        bool* _contained;
        unsigned int _containedCount;

    public:
        CastChunk(BaseChunk::IdType id, bool full = false) : id(id)
        {
            if (full)
            {
                _contained = 0;
                _containedCount = ChunkSize3;
            }
            else
            {
                _contained = new bool[ChunkSize3];
                std::memset(_contained, 0, ChunkSize3);
                _containedCount = 0;
            }
        }
        ~CastChunk()
        {
            Tools::Delete(this->_contained);
        }

        void AddCube(BaseChunk::CoordType x, BaseChunk::CoordType y, BaseChunk::CoordType z)
        {
            unsigned int index = x + y * ChunkSize + z * ChunkSize2;

            assert(this->_containedCount < ChunkSize3 && "already full");
            assert(this->_contained[index] == false && "cube already here");
            this->_contained[index] = true;
            ++this->_containedCount;
            if (this->_containedCount == ChunkSize3)
            {
                Tools::Delete(this->_contained);
                this->_contained = 0;
            }
        }

        bool IsEmpty() { return this->_containedCount == 0; }
        bool IsFull() { return this->_containedCount == ChunkSize3; }

        std::vector<BaseChunk::CoordsType> GetContained()
        {
            assert(this->_containedCount != ChunkSize3);

            std::vector<BaseChunk::CoordsType> res;

            unsigned int x, y, z;
             for (x = 0; x < ChunkSize; ++x)
                 for (y = 0; y < ChunkSize; ++y)
                     for (z = 0; z < ChunkSize; ++z)
                         if (this->_contained[x + y * ChunkSize + z * ChunkSize2])
                             res.push_back(BaseChunk::CoordsType(x, y, z));

            return res;
        }
    private:
        CastChunk(CastChunk const&);
        CastChunk& operator=(CastChunk const&);
    };

    class RayCast
    {
    public:
        static std::vector<Common::CubePosition> Ray(Common::OrientedPosition const& pos, float distance);
        static std::vector<CastChunk*> SphereArea(Common::Position const& pos, float distance);
        static std::vector<CastChunk*> CubeArea(Common::Position const& pos, float distance);
    };

}

#endif
