#ifndef __COMMON_NCHUNK_HPP__
#define __COMMON_NCHUNK_HPP__

#include "common/BaseChunk.hpp"

namespace Common {

    // Un N-Chunk, c'est un chunk de chunk dont le cote fait 2^N
    // Exemple :
    // Un 3-Chunk fait 8 chunks de cote
    // donc 512 chunks au total
    // Un 0-Chunk, c'est un chunk a la con
    // Le max, c'est un 20-Chunk, et ca fait la taille de la map.

    template<unsigned int N>
        struct NChunk
        {
            public:
                typedef BaseChunk::IdType IdType;
                typedef BaseChunk::CoordType CoordType;
                typedef BaseChunk::CoordsType CoordsType;

                enum
                {
                    chunkCount = (1 << N),
                    chunkCount1 = chunkCount,
                    chunkCount2 = chunkCount * chunkCount,
                    chunkCount3 = chunkCount * chunkCount * chunkCount,
                    pxMax = 22 - N,
                    pyMax = 20 - N,
                    pzMax = 22 - N,
                    xMax = 1 << pxMax,
                    yMax = 1 << pyMax,
                    zMax = 1 << pzMax,
                };

            public:
                NChunk(IdType id) :
                    id(id), coords(IdToCoords(id)), _cubes(0)
                {
                    assert(this->coords.x < (xMax));
                    assert(this->coords.y < (yMax));
                    assert(this->coords.z < (zMax));
                }

                static inline void IdToCoords(IdType id, CoordsType& coords)
                {
                    coords.x = (id                & ((xMax) - 1));
                    coords.y = ((id >> pxMax)        & ((yMax) - 1));
                    coords.z = ((id >> (pxMax + pyMax)) & ((zMax) - 1));
                }

                static inline CoordsType IdToCoords(IdType id)
                {
                    NChunk::CoordsType coords;
                    NChunk::IdToCoords(id, coords);
                    return coords;
                }

                static inline IdType CoordsToId(CoordType x, CoordType y, CoordType z)
                {
                    return (
                            ((IdType) (x)) +
                            (((IdType)(y)) << pxMax) +
                            (((IdType)(z)) << (pxMax + pyMax))
                           );
                }

                static inline IdType CoordsToId(CoordsType const& coord)
                {
                    return NChunk::CoordsToId(coord.x, coord.y, coord.z);
                }

        };

}

#endif
