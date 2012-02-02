
#include <time.h>

#include "common/BaseChunk.hpp"

#include "Assert.hpp"
using namespace Common;
typedef BaseChunk Chunk;

Chunk::IdType Rand64()
{
    return (
        ((Chunk::IdType) ((Uint32) rand())) +
        ((Chunk::IdType) ((Uint32) rand())) * ((Chunk::IdType) ((Uint32) -1))
    );
}

Chunk::CoordsType randCoords()
{
    auto coords = Chunk::CoordsType(
        (rand() % (1 << 24)) - (1 << 23),
        (rand() % (1 << 16)) - (1 << 15),
        (rand() % (1 << 24)) - (1 << 23)
    );
    Assert(coords.x < (1u << 23) && coords.x > -(1u << 23) - 1u);
    Assert(coords.y < (1u << 15) && coords.y > -(1u << 15) - 1u);
    Assert(coords.z < (1u << 23) && coords.z > -(1u << 23) - 1u);
    return coords;
}

int main()
{
    srand(time(0));
    for (unsigned int i = 0; i < 1000; ++i)
    {
        {
            Chunk::IdType id = Rand64();
            Chunk::CoordsType coords;
            Chunk::IdToCoords(id, coords);
            Assert(Chunk::CoordsToId(coords) == id);
        }
        {
            Chunk::CoordsType coords = randCoords();
            Chunk::IdType id = Chunk::CoordsToId(coords);
            //std::cout << ToString(Chunk::IdToCoords(id)) << " == " << ToString(coords) << std::endl;
            Assert(Chunk::IdToCoords(id) == coords);
        }
    }
    std::cout << "tests done.\n";
    return 0;
}


