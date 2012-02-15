#include "client2/precompiled.hpp"

#include "tools/Octree.hpp"
#include "client2/Client.hpp"
#include "client2/Settings.hpp"
#include "client2/game/Game.hpp"
#include "client2/map/ChunkManager.hpp"

namespace Client { namespace Map {

    ChunkManager::ChunkManager(Game::Game& game)
        : _game(game)
    {
        Tools::Vector3d coords;
        coords.y = 0;
        for (unsigned int x = 0 ; x < 4 ; ++x)
        {
            coords.x = x << (22 - 2);
            for (unsigned int z = 0 ; z < 4 ; ++z)
            {
                coords.z = z << (22 - 2);
                this->_octree[x + z * 4] = new Tools::Octree<ChunkNode>(coords * Common::ChunkSize, (1 << 20) * Common::ChunkSize);
            }
        }
    }

    ChunkManager::~ChunkManager()
    {
        for (size_t i = 0; i < sizeof(this->_octree)/sizeof(*this->_octree); ++i)
            Tools::Delete(this->_octree[i]);
    }

    void ChunkManager::AddChunk(std::unique_ptr<Chunk>&& chunk)
    {
        if (this->_downloadingChunks.find(chunk->id) != this->_downloadingChunks.end())
        {
            this->_downloadingChunks.erase(chunk->id);
            this->_chunks.insert(std::unordered_map<Common::BaseChunk::IdType, ChunkNode>::value_type(chunk->id, ChunkNode(std::move(chunk))));
        }
    }

    void ChunkManager::Update(Common::Position const& playerPosition)
    {
        this->_DownloadNewChunks(playerPosition);
        this->_RemoveOldChunks(playerPosition);
    }

    float ChunkManager::GetLoadingProgression() const
    {
        // TODO
        return 1.0f;
    }

    void ChunkManager::_RemoveOldChunks(Common::Position const& playerPosition)
    {
        Tools::AlignedCube cacheDistance(Tools::Vector3d(playerPosition.chunk), this->_game.GetClient().GetSettings().chunkCacheDistance * Common::ChunkSize * 2);
        for (size_t i = 0; i < sizeof(this->_octree)/sizeof(*this->_octree); ++i)
            this->_octree[i]->RemoveElementsOut(
                cacheDistance,
                [this](ChunkNode& node)
                {
                    this->_chunks.erase(node.chunk->id);
                });
    }

    void ChunkManager::_DownloadNewChunks(Common::Position const& playerPosition)
    {
        int cacheDistance = (int)this->_game.GetClient().GetSettings().chunkCacheDistance;
        int min = -cacheDistance * 80 / 100; // TODO: valeur dans les settings
        int max = cacheDistance * 80 / 100;

        Common::BaseChunk::CoordsType const& coords = playerPosition.world;
        Common::BaseChunk::CoordsType pos;
        Common::BaseChunk::IdType id;
        int x, y, z;
        for (x = min ; x < max ; ++x)
        {
            pos.x = coords.x + x;
            if (pos.x >= 1 << 22)
                continue;
            for (y = min ; y < max ; ++y)
            {
                pos.y = coords.y + y;
                if (pos.y >= 1 << 20)
                    continue;
                for (z = min ; z < max ; ++z)
                {
                    pos.z = coords.z + z;
                    if (pos.z >= 1 << 22)
                        continue;

                    id = Chunk::CoordsToId(pos);
                    if (this->_downloadingChunks.find(id) == this->_downloadingChunks.end() &&
                        this->_chunks.find(id) == this->_chunks.end())
                        this->_downloadingChunks.insert(id);
                }
            }
        }
    }

}}
