#ifndef __COMMON_BASECHUNK_HPP__
#define __COMMON_BASECHUNK_HPP__

#include "common/constants.hpp"
#include "tools/Vector3.hpp"

namespace Common {

    struct BaseChunk
    {
    public:
        typedef Uint16 CubeType;
        typedef Int64 IdType;
        typedef Uint32 CoordType;
        typedef Tools::Vector3<CoordType> CoordsType;

    public:
        IdType const id;
        CoordsType const coords;      // ChunkSize unit

    private:
        CubeType* _cubes;

    public:
        BaseChunk(IdType id) :
            id(id), coords(IdToCoords(id)), _cubes(0)
        {
            assert(this->coords.x < (1 << 23));
            assert(this->coords.y < (1 << 21));
            assert(this->coords.z < (1 << 23));
    //        assert(this->coords.x < (1 << 21) && this->coords.x > -(1 << 21) - 1);
    //        assert(this->coords.y < (1 << 19) && this->coords.y > -(1 << 19) - 1);
    //        assert(this->coords.z < (1 << 21) && this->coords.z > -(1 << 21) - 1);
        }

        BaseChunk(CoordsType const& c) :
            id(CoordsToId(c)), coords(c), _cubes(0)
        {
            assert(this->coords.x < (1 << 23));
            assert(this->coords.y < (1 << 21));
            assert(this->coords.z < (1 << 23));
    //        assert(this->coords.x < (1 << 21) && this->coords.x > -(1 << 21) - 1);
    //        assert(this->coords.y < (1 << 19) && this->coords.y > -(1 << 19) - 1);
    //        assert(this->coords.z < (1 << 21) && this->coords.z > -(1 << 21) - 1);
        }

        ~BaseChunk() // XXX destructor is not virtual
        {
            Tools::DeleteTab(this->_cubes);
        }

        inline bool IsEmpty() const
        {
            return this->_cubes == 0;
        }

        inline CubeType GetCube(CoordsType const& coords) const
        {
            return this->GetCube(coords.x, coords.y, coords.z);
        }

        inline CubeType GetCube(Uint32 x, Uint32 y, Uint32 z) const
        {
            assert(x < ChunkSize);
            assert(y < ChunkSize);
            assert(z < ChunkSize);
            if (this->_cubes == 0)
                return 0;
            return this->_cubes[x + y * ChunkSize + z * ChunkSize2];
        }

        inline void SetCube(CoordsType const& coords, CubeType type)
        {
            this->SetCube(coords.x, coords.y, coords.z, type);
        }

        inline void SetCube(Uint32 x, Uint32 y, Uint32 z, CubeType type)
        {
            assert(x < ChunkSize);
            assert(y < ChunkSize);
            assert(z < ChunkSize);
            if (this->_cubes == 0)
            {
                if (type == 0)
                    return;
                this->_cubes = new CubeType[ChunkSize3];
                std::memset(this->_cubes, 0, ChunkSize3 * sizeof(CubeType)); // vraiment ?
            }
            this->_cubes[x + y * ChunkSize + z * ChunkSize2] = type;
        }

        inline CubeType const* GetCubes() const
        {
            assert(this->_cubes != 0 && "GetCubes() called on empty BaseChunk");
            return this->_cubes;
        }

        inline void SetCubes(std::unique_ptr<CubeType>& cubes)
        {
            Tools::DeleteTab(this->_cubes);
            this->_cubes = cubes.release();
            assert(this->_cubes != 0 && "Ca va pas non ?");
        }

#ifdef _WIN32
# pragma warning(disable: 4244)
#endif

        static inline void IdToCoords(IdType id, BaseChunk::CoordsType& coords)
        {
            static_assert(sizeof(BaseChunk::IdType) == 8, "sizeof id type is 8");
            coords.x = (id                & ((1 << 22) - 1))/* - (1 << 21)*/;
            coords.y = ((id >> 22)        & ((1 << 20) - 1))/* - (1 << 19)*/;
            coords.z = ((id >> (22 + 20)) & ((1 << 22) - 1))/* - (1 << 21)*/;
        }

        static inline CoordsType IdToCoords(IdType id)
        {
            BaseChunk::CoordsType coords;
            BaseChunk::IdToCoords(id, coords);
            return coords;
        }

        static inline IdType CoordsToId(CoordType x, CoordType y, CoordType z)
        {
            return (
                    ((BaseChunk::IdType)  (x /*+ (1LL << 21)*/)) +
                    (((BaseChunk::IdType) (y /*+ (1LL << 19)*/)) << 22) +
                    (((BaseChunk::IdType) (z /*+ (1LL << 21)*/)) << (22 + 20))
                   );
        }

        static inline IdType CoordsToId(CoordsType const& coord)
        {
            return BaseChunk::CoordsToId(coord.x, coord.y, coord.z);
        }

    private:
        // non-copyable
        BaseChunk(BaseChunk const&);
        BaseChunk& operator =(BaseChunk const&);
    };

}

#endif
