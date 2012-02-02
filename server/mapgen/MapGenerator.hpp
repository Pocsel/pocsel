#ifndef __SERVER_MAPGEN_MAPGENERATOR_HPP__
# define __SERVER_MAPGEN_MAPGENERATOR_HPP__

# include "tools/MessageQueue.hpp"

# include "server/Chunk.hpp"

namespace Server {
    struct MapConf;
}

namespace Server { namespace MapGen {

    class CubeSpawnInfo;
    class Perlin;
    class Equation;

    class MapGenerator :
        public Tools::MessageQueue<MapGenerator>
    {
        private:
            std::list<CubeSpawnInfo*> _cubes;
            std::vector<Equation*> _equations;
            Perlin* _perlin;

        public:
            explicit MapGenerator(MapConf const& conf);
            ~MapGenerator();
            TOOLS_MQ_CBWRAP1(Chunk*, MapGenerator, GetChunk, Chunk::IdType, id, void(Chunk*));
    };

}}

#endif
