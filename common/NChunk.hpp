#ifndef __COMMON_NCHUNK_HPP__
#define __COMMON_NCHUNK_HPP__

#include "tools/Vector3.hpp"

namespace Common {

    // Un N-Chunk, c'est un chunk de chunk dont le cote fait 2^N
    // Exemple :
    // Un 3-Chunk fait 8 chunks de cote
    // donc 512 chunks au total
    // Un 0-Chunk, c'est un chunk a la con
    // Le Max1, c'est un 20-Chunk, et ca fait la taille de la map.

    template<unsigned int N>
        struct NChunk
        {
            public:
                typedef Int64 IdType;
                typedef Uint32 CoordType;
                typedef glm::detail::tvec3<CoordType> CoordsType;

                enum
                {
                    chunkCount = (1 << N),
                    chunkCount1 = chunkCount,
                    chunkCount2 = chunkCount * chunkCount,
                    chunkCount3 = chunkCount * chunkCount * chunkCount,
                    pxMax = 22 - N,
                    pyMax = 20 - N,
                    pzMax = 22 - N,
                    pxMax1 = pxMax + 1,
                    pyMax1 = pyMax + 1,
                    pzMax1 = pzMax + 1,
                    xMax1 = 1 << pxMax1,
                    yMax1 = 1 << pyMax1,
                    zMax1 = 1 << pzMax1,
                    xMax = 1 << pxMax,
                    yMax = 1 << pyMax,
                    zMax = 1 << pzMax,
                };

                static const IdType idMask =
                    (((IdType)xMax - 1) << (N)) |
                    (((IdType)yMax - 1) << (pxMax + N * 2)) |
                    (((IdType)zMax - 1) << (pxMax + pyMax + N * 3));

            public:
                IdType const id;
                CoordsType const coords;      // ChunkSize unit

            public:
                NChunk(IdType id) :
                    id(id), coords(IdToCoords(id))
                {
                    assert(this->coords.x < (xMax1));
                    assert(this->coords.y < (yMax1));
                    assert(this->coords.z < (zMax1));
                }

                NChunk(CoordsType const& c) :
                    id(CoordsToId(c)), coords(c)
                {
                    assert(this->coords.x < (xMax1));
                    assert(this->coords.y < (yMax1));
                    assert(this->coords.z < (zMax1));
                }

                ~NChunk()
                {
                }

#ifdef _WIN32
# pragma warning(disable: 4244)
#endif

                static void IdToCoords(IdType id, CoordsType& coords)
                {
                    coords.x = ((id >> (N))                     & ((xMax) - 1));
                    coords.y = ((id >> (pxMax + N * 2))         & ((yMax) - 1));
                    coords.z = ((id >> (pxMax + pyMax + N * 3)) & ((zMax) - 1));
                }

                static CoordsType IdToCoords(IdType id)
                {
                    NChunk::CoordsType coords;
                    NChunk::IdToCoords(id, coords);
                    return coords;
                }

                static IdType CoordsToId(CoordType x, CoordType y, CoordType z)
                {
                    return (
                            (((IdType)(x)) << (N)) +
                            (((IdType)(y)) << (pxMax + N * 2)) +
                            (((IdType)(z)) << (pxMax + pyMax + N * 3))
                           );
                }

                static IdType CoordsToId(CoordsType const& coord)
                {
                    return NChunk::CoordsToId(coord.x, coord.y, coord.z);
                }

                template<unsigned int Nother>
                    bool Contains(NChunk<Nother> const& other)
                    {
                        static_assert(N > Nother, "Cannot contain something bigger.");

                        return (other.id & idMask) == this->id;
                    }

                template<unsigned int Nother>
                    static std::vector<IdType> GetContainedIds(IdType id)
                    {
                        static_assert(N > Nother, "Cannot contain something bigger.");

                        unsigned int count = chunkCount / NChunk<Nother>::chunkCount;

                        std::vector<IdType> res(count * count * count);

                        IdType* resPtr = res.data();

                        id &= idMask;

                        for (IdType x = 0; x < count ; ++x)
                        {
                            for (IdType y = 0; y < count ; ++y)
                            {
                                for (IdType z = 0; z < count ; ++z)
                                {
                                    *resPtr++ = id |
                                        ((x) << (Nother)) |
                                        ((y) << (NChunk<Nother>::pxMax + (Nother) * 2)) |
                                        ((z) << (NChunk<Nother>::pxMax + NChunk<Nother>::pyMax + (Nother) * 3));
                                }
                            }
                        }

                        return res;
                    }

                static IdType GetId(IdType id)
                {
                    return id & idMask;
                }

        };

}

#endif
