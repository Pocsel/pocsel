#ifndef __COMMON_BASECHUNK_HPP__
#define __COMMON_BASECHUNK_HPP__

#include "common/constants.hpp"
#include "common/NChunk.hpp"

namespace Common {

    struct BaseChunk : public NChunk<0>
    {
    public:
        typedef Uint16 CubeType;

    private:
        std::shared_ptr<CubeType> _sharedCubes;
        CubeType* _cubes;

    public:
        BaseChunk(IdType id) :
            NChunk<0>(id),
            _cubes(0)
        {
        }

        BaseChunk(CoordsType const& c) :
            NChunk<0>(c),
            _cubes(0)
        {
        }

        ~BaseChunk() // XXX destructor is not virtual
        {
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
                this->_sharedCubes.reset(this->_cubes);
            }
            this->_cubes[x + y * ChunkSize + z * ChunkSize2] = type;
        }

        inline CubeType const* GetCubes() const
        {
            assert(this->_cubes != 0 && "GetCubes() called on empty BaseChunk");
            return this->_cubes;
        }

        inline std::shared_ptr<CubeType> GetSharedCubes()
        {
            return this->_sharedCubes;
        }

        inline void SetCubes(std::unique_ptr<CubeType> cubes)
        {
            this->_cubes = cubes.release();
            this->_sharedCubes.reset(this->_cubes);
        }

        inline void SetCubes(std::shared_ptr<CubeType> cubes)
        {
            this->_sharedCubes = cubes;
            this->_cubes = cubes.get();
        }

        inline std::shared_ptr<CubeType> StealCubes()
        {
            std::shared_ptr<CubeType> ret = this->_sharedCubes;
            this->_sharedCubes.reset();
            this->_cubes = 0;
            return ret;
        }

    private:
        // non-copyable
        BaseChunk(BaseChunk const&);
        BaseChunk& operator =(BaseChunk const&);
    };

}

#endif
