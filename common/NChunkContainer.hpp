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
                typedef typename Contained::IdType LittleIdType;
                typedef typename Contained::CoordType LittleCoordType;
                typedef typename Contained::CoordsType LittleCoordsType;

                enum
                {
                    countMax = Container::chunkCount / Contained::chunkCount,
                    countMax1 = countMax,
                    countMax2 = countMax * countMax,
                    countMax3 = countMax * countMax * countMax,
                };
                static const IdType idMask = Container::idMask;

            private:
                bool _contained[countMax3];
                unsigned int _containedCount;

            public:
                NChunkContainer(IdType id, bool full = false) :
                    NChunk<bigN>(id)
                {
                    static_assert(bigN > littleN, "Cannot contain something bigger");
                    std::memset(_contained, full, sizeof(_contained));
                    if (full)
                        _containedCount = countMax3;
                    else
                        _containedCount = 0;
                }

                NChunkContainer(CoordsType const& c, bool full = false) :
                    NChunk<bigN>(c)
                {
                    static_assert(bigN > littleN, "Cannot contain something bigger");
                    std::memset(_contained, full, sizeof(_contained));
                    if (full)
                        _containedCount = countMax3;
                    else
                        _containedCount = 0;
                }

                void Dump()
                {
                    std::cout << this->_containedCount << " a la place de " << countMax3 << "\n";
                }

                ~NChunkContainer()
                {
                }

                void AddChunk(IdType id)
                {
                    unsigned int index = _GetIndex(id);

                    assert(this->_contained[index] == false && "chunk already here");
                    this->_contained[index] = true;
                    ++this->_containedCount;
                }

                void RemoveChunk(IdType id)
                {
                    assert((id & idMask) == this->id && "This chunk does not fit here");

                    unsigned int index = _GetIndex(id);

                    assert(this->_contained[index] == true && "chunk already gone");
                    this->_contained[index] = false;
                    --this->_containedCount;
                }

                bool IsEmpty()
                {
                    return this->_containedCount == 0;
                }

                bool IsFull()
                {
                    return this->_containedCount == countMax3;
                }

                std::vector<LittleIdType> GetContained()
                {
                    std::vector<LittleIdType> ids(this->_containedCount);

                    IdType* resPtr = ids.data();

                    for (IdType x = 0; x < countMax ; ++x)
                    {
                        for (IdType y = 0; y < countMax ; ++y)
                        {
                            for (IdType z = 0; z < countMax ; ++z)
                            {
                                if (this->_contained[x + y * countMax1 + z * countMax2])
                                    *resPtr++ = this->id |
                                        ((x) << (littleN)) |
                                        ((y) << (Contained::pxMax + (littleN) * 2)) |
                                        ((z) << (Contained::pxMax + Contained::pyMax + (littleN) * 3));
                            }
                        }
                    }

                    return ids;
                }


            private:
                unsigned int _GetIndex(IdType id)
                {
                    assert((id & NChunk<bigN>::idMask) == this->id && "This chunk does not fit here!");

                    id &= ~idMask;

                    unsigned int x = ((id >> (littleN)) &                                           (countMax - 1));
                    unsigned int y = ((id >> (Contained::pxMax + littleN * 2)) &                    (countMax - 1));
                    unsigned int z = ((id >> (Contained::pxMax + Contained::pyMax + littleN * 3)) & (countMax - 1));

                    return x + y * countMax1 + z * countMax2;
                }

        };

}

#endif
