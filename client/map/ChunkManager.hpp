#ifndef __CLIENT_MAP_CHUNKMANAGER_HPP__
#define __CLIENT_MAP_CHUNKMANAGER_HPP__

#include "common/BaseChunk.hpp"
#include "common/Position.hpp"
#include "tools/AlignedCube.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/ChunkRenderer.hpp"

#include "tools/Octree.hpp"
#include "tools/thread/Task.hpp"

namespace Tools { namespace Thread {
    class ThreadPool;
}}

namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Map {

    class ChunkManager
        : private boost::noncopyable
    {
    private:
        class ChunkNode
            : public Tools::AlignedCube
        {
        public:
            std::shared_ptr<Chunk> chunk;

            ChunkNode(std::unique_ptr<Chunk>&& chunk)
                : Tools::AlignedCube(glm::dvec3(chunk->coords * Common::ChunkSize), Common::ChunkSize),
                chunk(std::move(chunk))
            {
            }
        };

    private:
        Game::Game& _game;
        ChunkRenderer _chunkRenderer;
        std::unordered_set<Common::BaseChunk::IdType> _downloadingChunks;
        std::unordered_map<Common::BaseChunk::IdType, ChunkNode*> _chunks;
        std::map<ChunkNode*, std::shared_ptr<Tools::Thread::Task<bool>>> _refreshTasks;
        Tools::Octree<ChunkNode>* _octree[16];
        float _loadingProgression;
        Common::Position _oldPosition;
        Tools::Thread::ThreadPool& _threadPool;

    public:
        ChunkManager(Game::Game& game);
        ~ChunkManager();

        void AddChunk(std::unique_ptr<Chunk>&& chunk);
        void UpdateLoading();
        void Update(Uint64 totalTime, Common::Position const& playerPosition);
        void Render() { this->_chunkRenderer.Render(); }
        void RenderAlpha() { this->_chunkRenderer.RenderAlpha(); }

        template<class TFunc>
        void ForeachIn(Tools::AbstractCollider const& container, TFunc function);
        std::shared_ptr<Chunk> GetChunk(Common::BaseChunk::IdType id) const;
        std::shared_ptr<Chunk> GetChunk(Common::BaseChunk::CoordsType const& coords) const { return this->GetChunk(Common::BaseChunk::CoordsToId(coords)); }
        float GetLoadingProgression() const { return this->_loadingProgression; }
    private:
        void _RemoveOldChunks(Common::Position const& playerPosition);
        void _DownloadNewChunks(Common::Position const& playerPosition);
        void _RefreshNode(ChunkNode& node);
        void _RefreshNode(ChunkNode& node, std::shared_ptr<Chunk::CubeType> oldCubes);
        void _AddNodeToRefresh(ChunkNode& node);
        bool _RefreshChunkMesh(std::shared_ptr<Chunk> chunk, std::shared_ptr<Chunk::CubeType> cubes, std::vector<Game::CubeType> cubeTypes, std::vector<std::shared_ptr<Chunk::CubeType>> neighbors);
    };

    template<class TFunc>
    void ChunkManager::ForeachIn(Tools::AbstractCollider const& container, TFunc function)
    {
        for (int i = 0; i < 16; ++i)
            this->_octree[i]->ForeachIn(container,
                [&function](ChunkNode& node)
                {
                    function(*node.chunk);
                });
    }

}}

#endif
