#ifndef __SERVER_GAME_MAP_GEN_CHUNKGENERATOR_HPP__
#define __SERVER_GAME_MAP_GEN_CHUNKGENERATOR_HPP__

#include "server/game/map/Chunk.hpp"

namespace Tools {

    class SimpleMessageQueue;

}

namespace Server { namespace Game { namespace Map {

    struct Conf;

}}}

namespace Server { namespace Game { namespace Map { namespace Gen {

    class CubeSpawnInfo;
    class Perlin;
    class IEquation;

    class ChunkGenerator :
        private boost::noncopyable
    {
    public:
        typedef std::function<void(Chunk*)> Callback;

    private:
        Tools::SimpleMessageQueue* _messageQueue;
        std::vector<CubeSpawnInfo> _cubes;
        std::vector<IEquation*> _equations;
        Perlin* _perlin;

    public:
        explicit ChunkGenerator(Conf const& conf);
        ~ChunkGenerator();

        void Start();
        void Stop();

        void GetChunk(Chunk::IdType id, Callback& callback);

    private:
        void _GetChunk(Chunk::IdType id, Callback& callback);
    };

}}}}

#endif
