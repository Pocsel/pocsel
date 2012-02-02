
#include <algorithm>
#include <iostream>

#include "client/World.hpp"
#include "client/Player.hpp"
#include "tools/gui/EventManager.hpp"
#include "tools/Timer.hpp"

using namespace Client;

World::World(Tools::Gui::EventManager& eventManager,
        Network& network,
        Tools::IRenderer& renderer,
        PacketDispatcher& packetDispatcher,
        Player& player) :
    _player(player),
    _renderer(renderer)
{
    eventManager.Connect(
            "player/moved",
            std::bind(&World::_OnPlayerMoved, this, std::placeholders::_1)
            );
}

World::~World()
{
}

Chunk const* World::GetChunk(Chunk::IdType id) const
{
    auto it = this->_chunks.find(id);
    if (it != this->_chunks.end())
        return it->second;
    return 0;
}

Chunk const* World::GetChunk(Common::BaseChunk::CoordsType const& chunkPosition) const
{
    return this->GetChunk(Common::BaseChunk::CoordsToId(chunkPosition));
}

Chunk* World::GetChunk(Chunk::IdType id)
{
    auto it = this->_chunks.find(id);
    if (it != this->_chunks.end())
        return it->second;
    return 0;
}

Chunk* World::GetChunk(Common::BaseChunk::CoordsType const& chunkPosition)
{
    return this->GetChunk(Common::BaseChunk::CoordsToId(chunkPosition));
}

Common::BaseChunk::CubeType World::GetCube(Common::BaseChunk::CoordsType const& chunkPosition, Tools::Vector3<Uint32> const& cubePosition) const
{
    auto chunk = this->GetChunk(chunkPosition);
    if (chunk == 0)
        return 0;
    return chunk->GetCube(cubePosition);
}

void World::_OnPlayerMoved(Tools::Gui::Event const&)
{
    auto p = this->_player.GetPosition();
}

void World::InsertChunk(std::unique_ptr<Chunk>& newChunk)
{
    Chunk& chunk = *newChunk.release();
    this->_chunks.erase(chunk.id);
    this->_chunks.insert(ChunkMap::value_type(chunk.id, &chunk));
    this->_octree.InsertChunk(chunk);
    this->_pendingDisplayChunk.push_back(&chunk);

    auto chunkLeft  = this->GetChunk(chunk.coords + Common::BaseChunk::CoordsType(-1,  0,  0)),
        chunkRight  = this->GetChunk(chunk.coords + Common::BaseChunk::CoordsType( 1,  0,  0)),
        chunkFront  = this->GetChunk(chunk.coords + Common::BaseChunk::CoordsType( 0,  0,  1)),
        chunkBack   = this->GetChunk(chunk.coords + Common::BaseChunk::CoordsType( 0,  0, -1)),
        chunkTop    = this->GetChunk(chunk.coords + Common::BaseChunk::CoordsType( 0,  1,  0)),
        chunkBottom = this->GetChunk(chunk.coords + Common::BaseChunk::CoordsType( 0, -1,  0));
    if (chunkLeft != 0) this->_pendingDisplayChunk.push_back(chunkLeft);
    if (chunkRight != 0) this->_pendingDisplayChunk.push_back(chunkRight);
    if (chunkFront != 0) this->_pendingDisplayChunk.push_back(chunkFront);
    if (chunkBack != 0) this->_pendingDisplayChunk.push_back(chunkBack);
    if (chunkTop != 0) this->_pendingDisplayChunk.push_back(chunkTop);
    if (chunkBottom != 0) this->_pendingDisplayChunk.push_back(chunkBottom);
    this->_pendingDisplayChunk.unique();
}

void World::InvalidateChunk(Chunk& chunk)
{
    this->_pendingDisplayChunk.push_back(&chunk);
}

void World::RenderChunks()
{
    if (!this->_pendingDisplayChunk.empty())
    {
        Tools::Timer timer;
        while (!this->_pendingDisplayChunk.empty() && timer.GetElapsedTime() < 10) // 10ms max environ
        {
            this->_pendingDisplayChunk.front()->RefreshDisp(this->_renderer, *this);
            this->_pendingDisplayChunk.pop_front();
        }
    }
    this->_octree.Render(this->_player.GetCamera(), this->_renderer);
}

std::deque<Chunk::IdType> World::GetNeededChunks()
{
    std::deque<Chunk::IdType> ids;

    /*
     * 16 * 16 * 16 histoire de se comparer à minecraft:
     * http://optifog.blogspot.com/2011/08/understanding-minecraft-performance.html
     */
    int min = -4;
    int max = 4;

    Chunk::CoordsType const& coords = this->_player.GetPosition().world;
    Chunk::CoordsType pos;
    Chunk::IdType id;
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
                if (this->_chunks.find(id) == this->_chunks.end())
                {
                    ids.push_back(id);
                    typedef Client::Chunk* Bite;
                    this->_chunks.insert(ChunkMap::value_type(id, Bite(0)));
                }
            }
        }
    }
    return ids;
}
