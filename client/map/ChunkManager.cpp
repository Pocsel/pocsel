#include "client/precompiled.hpp"

#include "tools/Octree.hpp"
#include "tools/Timer.hpp"
#include "tools/thread/ThreadPool.hpp"

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
        _loadingProgression(0),
        _threadPool(game.GetClient().GetThreadPool())
    {
        glm::dvec3 coords;
        coords.y = 0;
        for (unsigned int x = 0 ; x < 4 ; ++x)
        {
            coords.x = x << (22 - 2);
            for (unsigned int z = 0 ; z < 4 ; ++z)
            {
                coords.z = z << (22 - 2);
                this->_octree[x + z * 4] = new Tools::Octree<ChunkNode>(coords * (double)Common::ChunkSize, (1 << 20) * Common::ChunkSize);
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
        for (auto it = this->_refreshTasks.begin(), ite = this->_refreshTasks.end(); it != ite; ++it)
            it->second->Cancel();
        while (!this->_refreshTasks.empty())
        {
            boost::this_thread::yield();
            std::list<ChunkNode*> toDelete;
            for (auto it = this->_refreshTasks.begin(), ite = this->_refreshTasks.end(); it != ite; ++it)
                if (it->second->IsExecuted() && it->second.unique())
                    toDelete.push_back(it->first);
            std::for_each(toDelete.begin(), toDelete.end(), [this](ChunkNode* node) { this->_refreshTasks.erase(node); });
        }
        for (size_t i = 0; i < sizeof(this->_octree)/sizeof(*this->_octree); ++i)
            Tools::Delete(this->_octree[i]);
        for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    void ChunkManager::AddChunk(std::unique_ptr<Chunk>&& chunk)
    {
        ChunkNode* node = 0;

        std::shared_ptr<Chunk::CubeType> cubes(0);

        if (this->_chunks.find(chunk->id) != this->_chunks.end())
        { // Chunk Update
            node = this->_chunks.find(chunk->id)->second;
            cubes = node->chunk->GetSharedCubes();
            node->chunk->SetCubes(chunk->StealCubes());
        }
        else
        { // New chunk
            chunk->SetMesh(std::unique_ptr<ChunkMesh>(new ChunkMesh(*chunk)));

            int viewDistance = this->_game.GetClient().GetSettings().chunkViewDistance * 2;
            int nbChunks = viewDistance * viewDistance * viewDistance;
            this->_loadingProgression = std::min(1.0f,
                this->_loadingProgression +
                (0.5f / (float)(nbChunks))
                );

            this->_downloadingChunks.erase(chunk->id);
            node = new ChunkNode(std::move(chunk));
            this->_chunks.insert(std::unordered_map<Common::BaseChunk::IdType, ChunkNode*>::value_type(node->chunk->id, node));
            for (int i = 0; i < 16; ++i)
                if (this->_octree[i]->Contains(*node) == Tools::AbstractCollider::Inside)
                    this->_octree[i]->InsertElement(node);
        }

        if (node == 0)
            return;

        if (cubes == 0)
            this->_RefreshNode(*node);
        else
            this->_RefreshNode(*node, cubes);
    }

    void ChunkManager::UpdateLoading()
    {
        Tools::Timer timer;
        std::list<ChunkNode*> toDelete;
        for (auto it = this->_refreshTasks.begin(), ite = this->_refreshTasks.end(); it != ite && timer.GetElapsedTime() <= 10; ++it)
        {
            if (it->second->IsExecuted() && it->second.unique())
            {
                if (it->second->GetResult())
                {
                    this->_chunkRenderer.RefreshGraphics(*it->first->chunk);

                    int viewDistance = this->_game.GetClient().GetSettings().chunkViewDistance * 2 - 2;
                    int nbChunks = viewDistance * viewDistance * viewDistance;
                    this->_loadingProgression = std::min(1.0f,
                        this->_loadingProgression +
                        (0.5f / (float)(nbChunks))
                        );
                }
                else if (!it->second->IsCancelled())
                    this->_AddNodeToRefresh(*it->first);
                toDelete.push_back(it->first);
            }
        }
        std::for_each(toDelete.begin(), toDelete.end(), [this](ChunkNode* node) { this->_refreshTasks.erase(node); });
        if (this->_loadingProgression > 0.99f)
            this->_loadingProgression = 1.0f;
    }

    void ChunkManager::Update(Uint64 totalTime, Common::Position const& playerPosition)
    {
        Tools::Timer timer;
        std::list<ChunkNode*> toDelete;
        for (auto it = this->_refreshTasks.begin(), ite = this->_refreshTasks.end(); it != ite && timer.GetElapsedTime() <= 10; ++it)
        {
            if (it->second->IsExecuted() && it->second.unique())
            {
                if (it->second->GetResult() && !it->second->IsCancelled())
                    this->_chunkRenderer.RefreshGraphics(*it->first->chunk);
                else if (!it->second->IsCancelled())
                    this->_AddNodeToRefresh(*it->first);
                toDelete.push_back(it->first);
            }
        }
        std::for_each(toDelete.begin(), toDelete.end(), [this](ChunkNode* node) { this->_refreshTasks.erase(node); });
        this->_DownloadNewChunks(playerPosition);
        this->_RemoveOldChunks(playerPosition);
        this->_chunkRenderer.Update(totalTime);
        this->_oldPosition = playerPosition;
    }

    std::shared_ptr<Chunk> ChunkManager::GetChunk(Common::BaseChunk::IdType id) const
    {
        auto it = this->_chunks.find(id);
        if (it == this->_chunks.end())
            return 0;
        return it->second->chunk;
    }

    void ChunkManager::_RemoveOldChunks(Common::Position const& playerPosition)
    {
        if (Common::GetChunkCoords(this->_oldPosition) == Common::GetChunkCoords(playerPosition))
            return;
        unsigned int nbChunks = this->_game.GetClient().GetSettings().chunkViewDistance
            + this->_game.GetClient().GetSettings().chunkCacheArea;
        glm::dvec3 pos((Common::GetChunkCoords(playerPosition) - glm::uvec3(nbChunks)) * Common::ChunkSize);
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
            {
                auto taskIt = this->_refreshTasks.find(*it);
                if (taskIt != this->_refreshTasks.end())
                {
                    taskIt->second->Cancel();
                }
                this->_octree[i]->RemoveElement(*it);
                this->_chunks.erase((*it)->chunk->id);
                // this->_refreshTasks.erase(*it);
                Tools::Delete(*it);
            }
        }
    }

    void ChunkManager::_DownloadNewChunks(Common::Position const& playerPosition)
    {
        if (Common::GetChunkCoords(this->_oldPosition) == Common::GetChunkCoords(playerPosition))
            return;
        std::deque<Common::BaseChunk::IdType> tmp;
        int cacheDistance = (int)this->_game.GetClient().GetSettings().chunkViewDistance;
        int min = -cacheDistance;
        int max = cacheDistance;

        Common::BaseChunk::CoordsType const& coords = Common::GetChunkCoords(playerPosition);
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
        if (chunkLeft != this->_chunks.end()) this->_AddNodeToRefresh(*chunkLeft->second);
        if (chunkRight != this->_chunks.end()) this->_AddNodeToRefresh(*chunkRight->second);
        if (chunkFront != this->_chunks.end()) this->_AddNodeToRefresh(*chunkFront->second);
        if (chunkBack != this->_chunks.end()) this->_AddNodeToRefresh(*chunkBack->second);
        if (chunkTop != this->_chunks.end()) this->_AddNodeToRefresh(*chunkTop->second);
        if (chunkBottom != this->_chunks.end()) this->_AddNodeToRefresh(*chunkBottom->second);
        this->_AddNodeToRefresh(node);
    }

#ifdef _MSC_VER
#pragma warning(disable: 4307 4308)
#endif
    namespace {
        template<int x0, int y0, int z0>
            inline bool CheckModif(Chunk::CubeType const* oldCubes, Chunk::CubeType const* newCubes)
            {
                static_assert(x0 == 0 || x0 == Common::ChunkSize - 1 ||
                              y0 == 0 || y0 == Common::ChunkSize - 1 ||
                              z0 == 0 || z0 == Common::ChunkSize - 1,
                              "need a correct value");

                if (newCubes == 0)
                    return true;

                if (x0 != -1)
                {
                    unsigned int x = x0;
                    unsigned int y, z;
                    for (z = 0; z < Common::ChunkSize * Common::ChunkSize2; z += Common::ChunkSize2)
                    {
                        for (y = 0; y < Common::ChunkSize * Common::ChunkSize; y += Common::ChunkSize)
                        {
                            if (oldCubes[x + y + z] != newCubes[x + y + z])
                                return true;
                        }
                    }
                }
                else if (y0 != -1)
                {
                    unsigned int y = y0 * Common::ChunkSize;
                    unsigned int x, z;
                    for (z = 0; z < Common::ChunkSize * Common::ChunkSize2; z += Common::ChunkSize2)
                    {
                        for (x = 0; x < Common::ChunkSize; x += 1)
                        {
                            if (oldCubes[x + y + z] != newCubes[x + y + z])
                                return true;
                        }
                    }
                }
                else
                {
                    unsigned int z = z0 * Common::ChunkSize2;
                    unsigned int x, y;
                    for (y = 0; y < Common::ChunkSize * Common::ChunkSize; y += Common::ChunkSize)
                    {
                        for (x = 0; x < Common::ChunkSize; x += 1)
                        {
                            if (oldCubes[x + y + z] != newCubes[x + y + z])
                                return true;
                        }
                    }
                }
                return false;
            }
    }

    void ChunkManager::_RefreshNode(ChunkNode& node, std::shared_ptr<Chunk::CubeType> oldCubes)
    {
        static Chunk::CubeType* voidCubes = 0;
        if (voidCubes == 0)
        {
            voidCubes = new Chunk::CubeType[Common::ChunkSize3];
            std::memset(voidCubes, 0, sizeof(*voidCubes) * Common::ChunkSize3);
        }

        Chunk::CubeType const* newCubes;
        if (node.chunk->IsEmpty())
            newCubes = voidCubes;
        else
            newCubes = node.chunk->GetCubes();

        if (CheckModif<0, -1, -1>(oldCubes.get(), newCubes))
        {
            auto chunkLeft  = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType(-1,  0,  0)));
            if (chunkLeft != this->_chunks.end()) this->_AddNodeToRefresh(*chunkLeft->second);
        }
        if (CheckModif<Common::ChunkSize - 1, -1, -1>(oldCubes.get(), newCubes))
        {
            auto chunkRight  = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 1,  0,  0)));
            if (chunkRight != this->_chunks.end()) this->_AddNodeToRefresh(*chunkRight->second);
        }
        if (CheckModif<-1, -1, Common::ChunkSize - 1>(oldCubes.get(), newCubes))
        {
            auto chunkFront  = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0,  0,  1)));
            if (chunkFront != this->_chunks.end()) this->_AddNodeToRefresh(*chunkFront->second);
        }
        if (CheckModif<-1, -1, 0>(oldCubes.get(), newCubes))
        {
            auto chunkBack   = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0,  0, -1)));
            if (chunkBack != this->_chunks.end()) this->_AddNodeToRefresh(*chunkBack->second);
        }
        if (CheckModif<-1, Common::ChunkSize - 1, -1>(oldCubes.get(), newCubes))
        {
            auto chunkTop    = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0,  1,  0)));
            if (chunkTop != this->_chunks.end()) this->_AddNodeToRefresh(*chunkTop->second);
        }
        if (CheckModif<-1, 0, -1>(oldCubes.get(), newCubes))
        {
            auto chunkBottom = this->_chunks.find(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0, -1,  0)));
            if (chunkBottom != this->_chunks.end()) this->_AddNodeToRefresh(*chunkBottom->second);
        }
        this->_AddNodeToRefresh(node);
    }

    void ChunkManager::_AddNodeToRefresh(ChunkNode& node)
    {
        auto it = this->_refreshTasks.find(&node);
        if (it != this->_refreshTasks.end())
            it->second->Cancel();
        std::vector<std::shared_ptr<Chunk::CubeType>> neighbors(6);

        auto chk = this->GetChunk(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType(-1,  0,  0)));
        if (chk != 0)
            neighbors[0] = chk->GetSharedCubes();
        else return;
        chk = this->GetChunk(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 1,  0,  0)));
        if (chk != 0)
            neighbors[1] = chk->GetSharedCubes();
        else return;
        chk = this->GetChunk(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0,  0,  1)));
        if (chk != 0)
            neighbors[2] = chk->GetSharedCubes();
        else return;
        chk = this->GetChunk(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0,  0, -1)));
        if (chk != 0)
            neighbors[3] = chk->GetSharedCubes();
        else return;
        chk = this->GetChunk(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0,  1,  0)));
        if (chk != 0)
            neighbors[4] = chk->GetSharedCubes();
        else return;
        chk = this->GetChunk(Common::BaseChunk::CoordsToId(node.chunk->coords + Common::BaseChunk::CoordsType( 0, -1,  0)));
        if (chk != 0)
            neighbors[5] = chk->GetSharedCubes();
        else return;

        auto const& cubeTypes = this->_game.GetCubeTypeManager().GetCubeTypes();
        std::shared_ptr<Tools::Thread::Task<bool>>
            t(new Tools::Thread::Task<bool>(std::bind(&ChunkManager::_RefreshChunkMesh,
                this,
                node.chunk,
                node.chunk->GetSharedCubes(),
                std::move(cubeTypes),
                std::move(neighbors))));

        this->_threadPool.PushTask<bool>(t);
        this->_refreshTasks[&node] = t;
    }

    bool ChunkManager::_RefreshChunkMesh(std::shared_ptr<Chunk> chunk, std::shared_ptr<Chunk::CubeType> cubes, std::vector<Game::CubeType> cubeTypes, std::vector<std::shared_ptr<Chunk::CubeType>> neighbors)
    {
        return chunk->GetMesh()->Refresh(this->_chunkRenderer, cubeTypes, cubes, neighbors);
    }

}}
