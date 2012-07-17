#ifndef __COMMON_BASECHUNK_HPP__
#define __COMMON_BASECHUNK_HPP__

#include "common/constants.hpp"
#include "common/NChunk.hpp"

namespace Common {

    struct BaseChunk : public NChunk<0>
    {
    public:
        typedef Uint16 CubeType;
        typedef std::array<CubeType, Common::ChunkSize3> CubeArray;

    private:
        std::shared_ptr<CubeArray> _sharedCubes;

    public:
        explicit BaseChunk(IdType id) :
            NChunk<0>(id)
        {
        }

        explicit BaseChunk(CoordsType const& c) :
            NChunk<0>(c)
        {
        }

        ~BaseChunk() // XXX destructor is not virtual
        {
        }

        inline bool IsEmpty() const
        {
            return this->_sharedCubes.get() == 0;
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
            if (this->IsEmpty())
                return 0;
            return this->_sharedCubes->data()[x + y * ChunkSize + z * ChunkSize2];
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
            if (this->_sharedCubes.get() == 0)
            {
                if (type == 0)
                    return;
                this->_sharedCubes.reset(new CubeArray);
                std::memset(this->_sharedCubes->data(), 0, ChunkSize3 * sizeof(CubeType)); // vraiment ?
            }
            this->_sharedCubes->data()[x + y * ChunkSize + z * ChunkSize2] = type;
        }

        inline CubeType const* GetCubes() const
        {
            assert(this->_sharedCubes.get() != 0 && "GetCubes() called on empty BaseChunk");
            return this->_sharedCubes->data();
        }

        inline std::shared_ptr<CubeArray> GetSharedCubes()
        {
            return this->_sharedCubes;
        }

        inline void SetCubes(std::unique_ptr<CubeArray> cubes)
        {
            this->_sharedCubes.reset(cubes.release());
        }

        inline void SetCubes(std::shared_ptr<CubeArray> cubes)
        {
            this->_sharedCubes = cubes;
        }

        inline std::shared_ptr<CubeArray> StealCubes()
        {
            std::shared_ptr<CubeArray> ret = this->_sharedCubes;
            this->_sharedCubes.reset();
            return ret;
        }

    private:
        // non-copyable
        BaseChunk(BaseChunk const&);
        BaseChunk& operator =(BaseChunk const&);
    };

}

#endif
