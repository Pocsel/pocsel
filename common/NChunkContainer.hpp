#ifndef __COMMON_NCHUNKCONTAINER_HPP__
#define __COMMON_NCHUNKCONTAINER_HPP__

#include "common/NChunk.hpp"

namespace Common {

    template<unsigned int bigN, unsigned int littleN>
        struct NChunkContainer :
            public NChunk<bigN>
        {
            public:
                typedef NChunk<littleN> Contained;
                typedef NChunk<bigN> Container;
                typedef typename Container::IdType IdType;
                typedef typename Container::CoordType CoordType;
                typedef typename Container::CoordsType CoordsType;

                enum
                {
                    countMax = Container::chunkCount / Contained::chunkCount,
                    countMax1 = countMax,
                    countMax2 = countMax * countMax,
                    countMax3 = countMax * countMax * countMax,
                    idMask = Container::idMask
                };

            private:
                Uint8 _contained[countMax3];
                unsigned int _containedCount;

            public:
                NChunkContainer(IdType id, bool full = false) :
                    NChunk<bigN>(id)
                {
                    static_assert(bigN > littleN, "Cannot contain something bigger");
                    std::memset(_contained, full, sizeof(_contained));
                    if (full)
                        _containedCount = countMax3;
                }

                NChunkContainer(CoordsType const& c, bool full = false) :
                    NChunk<bigN>(c)
                {
                    static_assert(bigN > littleN, "Cannot contain something bigger");
                    std::memset(_contained, full, sizeof(_contained));
                    if (full)
                        _containedCount = countMax3;
                }

                ~NChunkContainer()
                {
                }

                void AddChunk(IdType id)
                {
                    unsigned int index = _GetIndex(id);

                    assert(this->_contained[index] == 0 && "chunk already here !");
                    this->_contained[index] = 1;
                    ++this->_containedCount;
                }

                void RemoveChunk(IdType id)
                {
                    assert((id & idMask) == this->id && "This chunk does not fit here!");

                    unsigned int index = _GetIndex(id);

                    assert(this->_contained[index] == 0 && "chunk already here !");
                    this->_contained[index] = 1;
                    ++this->_containedCount;
                }

                bool IsEmpty()
                {
                    return this->_containedCount == 0;
                }

                bool IsFull()
                {
                    return this->_containedCount == countMax3;
                }

            private:
                unsigned int _GetIndex(IdType id)
                {
                    assert((id & idMask) == this->id && "This chunk does not fit here!");

                    id &= ~idMask;

                    unsigned int x = ((id >> (littleN)) &                                           (countMax - 1));
                    unsigned int y = ((id >> (Contained::pxMax + littleN * 2)) &                    (countMax - 1));
                    unsigned int z = ((id >> (Contained::pxMax + Contained::pyMax + littleN * 3)) & (countMax - 1));

                    return x + y * countMax1 + z * countMax2;
                }

        };

}

#endif
