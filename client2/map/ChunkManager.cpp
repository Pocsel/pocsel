#include "client2/precompiled.hpp"

#include "tools/Octree.hpp"
#include "client2/Client.hpp"
#include "client2/Settings.hpp"
#include "client2/game/Game.hpp"
#include "client2/map/ChunkManager.hpp"
#include "client2/network/PacketCreator.hpp"

namespace Client { namespace Map {

    ChunkManager::ChunkManager(Game::Game& game)
        : _game(game),
        _chunkRenderer(game),
        _loadingProgression(0)
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
        this->_loadingProgression = std::max(1.0f, this->_loadingProgression + 0.01f);
        auto id = chunk->id;
        if (this->_downloadingChunks.find(id) != this->_downloadingChunks.end())
        {
            this->_downloadingChunks.erase(id);
            this->_chunks.insert(std::unordered_map<Common::BaseChunk::IdType, ChunkNode>::value_type(id, ChunkNode(std::move(chunk))));
            auto elem = &this->_chunks.find(id)->second;
            for (int i = 0; i < 16; ++i)
                if (this->_octree[i]->Contains(*elem) == Tools::AbstractCollider::Inside)
                    this->_octree[i]->InsertElement(elem);
            this->_waitingRefresh.push_back(elem);
        }
    }

    void ChunkManager::Update(Common::Position const& playerPosition)
    {
        int nb = 0;
        while (!this->_waitingRefresh.empty() && ++nb < 20)
        {
            this->_chunkRenderer.RefreshDisplay(*this->_waitingRefresh.front()->chunk);
            this->_waitingRefresh.pop_front();
        }
        this->_DownloadNewChunks(playerPosition);
        this->_RemoveOldChunks(playerPosition);
    }

    void ChunkManager::Render()
    {
        this->_chunkRenderer.Render();
    }

    Chunk* ChunkManager::GetChunk(Common::BaseChunk::IdType id) const
    {
        auto it = this->_chunks.find(id);
        if (it == this->_chunks.end())
            return 0;
        return it->second.chunk;
    }

    void ChunkManager::_RemoveOldChunks(Common::Position const& playerPosition)
    {
        return; // C'EST BUGGE
        unsigned int nbChunks = this->_game.GetClient().GetSettings().chunkViewDistance
            + this->_game.GetClient().GetSettings().chunkCacheArea;
        Tools::Vector3d pos((playerPosition.world - Tools::Vector3u(nbChunks)) * Common::ChunkSize);
        Tools::AlignedCube cacheArea(pos, nbChunks * Common::ChunkSize * 2);
        for (size_t i = 0; i < sizeof(this->_octree)/sizeof(*this->_octree); ++i)
        {
            std::deque<ChunkNode*> nodeToDelete;
            this->_octree[i]->ForeachOut(
                cacheArea,
                [&nodeToDelete](ChunkNode& node)
                {
                    nodeToDelete.push_back(&node);
                });
            for (auto it = nodeToDelete.begin(), ite = nodeToDelete.end(); it != ite; ++it)
                if (!this->_octree[i]->RemoveElement(*it))
                    Tools::log << "AAAAAAAAHHHHHHHHH\n";
        }
    }

    void ChunkManager::_DownloadNewChunks(Common::Position const& playerPosition)
    {
        std::deque<Common::BaseChunk::IdType> tmp;
        int cacheDistance = (int)this->_game.GetClient().GetSettings().chunkViewDistance;
        int min = -cacheDistance;
        int max = cacheDistance;

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
                    {
                        tmp.push_back(id);
                        this->_downloadingChunks.insert(id);
                    }
                }
            }
        }
        while (!tmp.empty())
            this->_game.GetClient().GetNetwork().SendPacket(Network::PacketCreator::NeedChunks(tmp));
    }

}}
