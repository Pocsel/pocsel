#ifndef __CLIENT_MAP_CHUNKMANAGER_HPP__
#define __CLIENT_MAP_CHUNKMANAGER_HPP__

#include "common/BaseChunk.hpp"
#include "common/Position.hpp"
#include "tools/AlignedCube.hpp"
#include "client2/map/Chunk.hpp"
#include "client2/map/ChunkRenderer.hpp"

#include "tools/Octree.hpp"

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
            ~ChunkNode()
            {
                Tools::Delete(this->chunk);
            }
        };

    private:
        Game::Game& _game;
        ChunkRenderer _chunkRenderer;
        std::unordered_set<Common::BaseChunk::IdType> _downloadingChunks;
        std::unordered_map<Common::BaseChunk::IdType, ChunkNode*> _chunks;
        std::list<ChunkNode*> _waitingRefresh;
        Tools::Octree<ChunkNode>* _octree[16];
        float _loadingProgression;

    public:
        ChunkManager(Game::Game& game);
        ~ChunkManager();

        void AddChunk(std::unique_ptr<Chunk>&& chunk);
        void UpdateLoading();
        void Update(Common::Position const& playerPosition);
        void Render();

        template<class TFunc>
        void ForeachIn(Tools::AbstractCollider const& container, TFunc function);
        Chunk* GetChunk(Common::BaseChunk::IdType id) const;
        Chunk* GetChunk(Common::BaseChunk::CoordsType const& coords) const { return this->GetChunk(Common::BaseChunk::CoordsToId(coords)); }
        float GetLoadingProgression() const { return this->_loadingProgression; }
    private:
        void _RemoveOldChunks(Common::Position const& playerPosition);
        void _DownloadNewChunks(Common::Position const& playerPosition);
        void _RefreshNode(ChunkNode& node);
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
