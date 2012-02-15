#ifndef __CLIENT_MAP_CHUNKMANAGER_HPP__
#define __CLIENT_MAP_CHUNKMANAGER_HPP__

#include "common/BaseChunk.hpp"
#include "common/Position.hpp"
#include "tools/AlignedCube.hpp"
#include "client2/map/Chunk.hpp"

namespace Tools {
    template<class T> class Octree;
}
namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Map {

    class ChunkManager
        : private boost::noncopyable
    {
    private:
        class ChunkNode
            : public Tools::AlignedCube,
            private boost::noncopyable
        {
        public:
            Chunk* chunk;

            ChunkNode(std::unique_ptr<Chunk> chunk)
                : Tools::AlignedCube(Tools::Vector3d(chunk->coords * Common::ChunkSize), Common::ChunkSize),
                chunk(chunk.release())
            {
            }
            ChunkNode(ChunkNode&& node)
                : Tools::AlignedCube(node),
                chunk(node.chunk)
            {
                node.chunk = 0;
            }
            ~ChunkNode()
            {
                Tools::Delete(this->chunk);
            }
        };

    private:
        Game::Game& _game;
        std::unordered_set<Common::BaseChunk::IdType> _downloadingChunks;
        std::unordered_map<Common::BaseChunk::IdType, ChunkNode> _chunks;
        Tools::Octree<ChunkNode>* _octree[16];

    public:
        ChunkManager(Game::Game& game);
        ~ChunkManager();

        void AddChunk(std::unique_ptr<Chunk>&& chunk);
        void Update(Common::Position const& playerPosition);

        float GetLoadingProgression() const;
    private:
        void _RemoveOldChunks(Common::Position const& playerPosition);
        void _DownloadNewChunks(Common::Position const& playerPosition);
    };

}}

#endif
