#include "client/precompiled.hpp"

#include "tools/Octree.hpp"
#include "tools/Timer.hpp"
#include "client/Client.hpp"
#include "client/Settings.hpp"
#include "client/game/Game.hpp"
#include "client/map/ChunkManager.hpp"
#include "client/network/PacketCreator.hpp"

#include "client/window/Window.hpp"
#include "client/window/InputManager.hpp"

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

        game.GetClient().GetWindow().GetInputManager().Bind("dump-octree", BindAction::Released,
            [this]()
            {
                for (size_t i = 0; i < sizeof(this->_octree)/sizeof(*this->_octree); ++i)
                    this->_octree[i]->Dump(Tools::log);
            });
    }

    ChunkManager::~ChunkManager()
    {
        for (size_t i = 0; i < sizeof(this->_octree)/sizeof(*this->_octree); ++i)
            Tools::Delete(this->_octree[i]);
        for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    void ChunkManager::AddChunk(std::unique_ptr<Chunk>&& chunk)
    {
        ChunkNode* node = 0;

        if (this->_chunks.find(chunk->id) != this->_chunks.end())
        { // Chunk Update
            node = this->_chunks.find(chunk->id)->second;
            delete node->chunk;
            node->chunk = chunk.release();
        }
        else
        { // New chunk
            this->_loadingProgression = std::min(1.0f, this->_loadingProgression + 0.0005f);
            this->_downloadingChunks.erase(chunk->id);
            node = new ChunkNode(std::move(chunk));
            this->_chunks.insert(std::unordered_map<Common::BaseChunk::IdType, ChunkNode*>::value_type(node->chunk->id, node));
            for (int i = 0; i < 16; ++i)
                if (this->_octree[i]->Contains(*node) == Tools::AbstractCollider::Inside)
                    this->_octree[i]->InsertElement(node);
        }

        if (node == 0)
            return;
        this->_RefreshNode(*node);
    }

    void ChunkManager::UpdateLoading()
    {
        Tools::Timer timer;
        this->_waitingRefresh.unique();
        while (!this->_waitingRefresh.empty() && timer.GetElapsedTime() < 30)
        {
            this->_loadingProgression = std::min(1.0f, this->_loadingProgression + 0.0002f);
            this->_chunkRenderer.RefreshDisplay(*this->_waitingRefresh.front()->chunk);
            this->_waitingRefresh.pop_front();
        }
        if (this->_waitingRefresh.empty() && this->_loadingProgression > 0.6f)
            this->_loadingProgression = 1.0f;
    }

    void ChunkManager::Update(Common::Position const& playerPosition)
    {
        Tools::Timer timer;
        while (!this->_waitingRefresh.empty() && timer.GetElapsedTime() < 10)
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
        return it->second->chunk;
    }

    void ChunkManager::_RemoveOldChunks(Common::Position const& playerPosition)
    {
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
                if (this->_octree[i]->RemoveElement(*it))
                {
                    this->_chunks.erase((*it)->chunk->id);
                    this->_waitingRefresh.remove(*it);
                    Tools::Delete(*it);
                }
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

    void ChunkManager::_RefreshNode(ChunkNode& node)
    {
        auto chunkLeft  = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType(-1,  0,  0))),
            chunkRight  = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 1,  0,  0))),
            chunkFront  = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0,  0,  1))),
            chunkBack   = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0,  0, -1))),
            chunkTop    = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0,  1,  0))),
            chunkBottom = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0, -1,  0)));
        this->_waitingRefresh.push_back(&node);
        if (chunkLeft != this->_chunks.end()) this->_waitingRefresh.push_back(chunkLeft->second);
        if (chunkRight != this->_chunks.end()) this->_waitingRefresh.push_back(chunkRight->second);
        if (chunkFront != this->_chunks.end()) this->_waitingRefresh.push_back(chunkFront->second);
        if (chunkBack != this->_chunks.end()) this->_waitingRefresh.push_back(chunkBack->second);
        if (chunkTop != this->_chunks.end()) this->_waitingRefresh.push_back(chunkTop->second);
        if (chunkBottom != this->_chunks.end()) this->_waitingRefresh.push_back(chunkBottom->second);
        if (this->_game.GetClient().GetState() != Client::LoadingChunks)
            this->_waitingRefresh.unique();
    }

}}
