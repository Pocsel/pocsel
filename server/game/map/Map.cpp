#include "server/game/map/Map.hpp"
#include "server/game/map/ChunkManager.hpp"

#include "common/CubeType.hpp"
#include "common/Position.hpp"
#include "common/CubePosition.hpp"
#include "common/Packet.hpp"

#include "tools/SimpleMessageQueue.hpp"

#include "server/Server.hpp"
#include "server/clientmanagement/ClientManager.hpp"
#include "server/game/Game.hpp"
#include "server/game/Player.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/map/gen/ChunkGenerator.hpp"
#include "server/network/PacketCreator.hpp"

namespace Server { namespace Game { namespace Map {

    Map::Map(Conf const& conf, Game& game) :
        _conf(conf),
        _game(game),
        _messageQueue(new Tools::SimpleMessageQueue(1)),
        _spawnPosition(0)
    {
        Tools::debug << "Map::Map() -- " << this->_conf.name << "\n";
        this->_gen = new Gen::ChunkGenerator(this->_conf);
        this->_engine = new Engine::Engine();
        this->_chunkManager = new ChunkManager();
    }

    Map::~Map()
    {
        Tools::debug << "Map::~Map() -- " << this->_conf.name << "\n";
        Tools::Delete(this->_gen);
        Tools::Delete(this->_engine);
        Tools::Delete(this->_messageQueue);
        Tools::Delete(this->_chunkManager);
    }

    void Map::Start()
    {
        Tools::debug << "Map::Start() -- " << this->_conf.name << "\n";
        this->_gen->Start();
        this->_messageQueue->Start();

        Tools::SimpleMessageQueue::TimerLoopMessage
            m(std::bind(&Map::_Tick, this, std::placeholders::_1));
        this->_messageQueue->SetLoopTimer(10000, m);
    }

    void Map::Stop()
    {
        Tools::debug << "Map::Stop() -- " << this->_conf.name << "\n";
        this->_gen->Stop();
        this->_messageQueue->Stop();
    }

    void Map::HandleNewChunk(Chunk* chunk)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_HandleNewChunk, this, chunk));
        this->_messageQueue->PushMessage(m);
    }

    void Map::GetSpawnPosition(SpawnCallback& response)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_GetSpawnPosition, this, response));
        this->_messageQueue->PushMessage(m);
    }

    void Map::GetChunk(Chunk::IdType id, ChunkCallback& response)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_GetChunk, this, id, response));
        this->_messageQueue->PushMessage(m);
    }

    void Map::GetChunkPacket(Chunk::IdType id, ChunkPacketCallback& response)
    {
        ChunkCallback ccb(std::bind(&Map::_SendChunkPacket, this, std::placeholders::_1, std::move(response)));
        this->GetChunk(id, ccb);
    }

    void Map::DestroyCube(Common::CubePosition const& pos)
    {
        ChunkCallback cb(std::bind(&Map::_DestroyCube, this, std::placeholders::_1, pos.chunk));
        this->GetChunk(Chunk::CoordsToId(pos.world), cb);
    }

    void Map::DestroyCubes(std::vector<Common::CubePosition> const& pos)
    {
        std::map<Chunk::IdType, std::vector<Chunk::CoordsType>> positions;

        std::for_each(pos.begin(), pos.end(), [&positions](Common::CubePosition const& p)
            {
                positions[Chunk::CoordsToId(p.world)].push_back(p.chunk);
            }
            );

        std::for_each(positions.begin(), positions.end(), [this](std::pair<Chunk::IdType const, std::vector<Chunk::CoordsType>>& p)
            {
                ChunkCallback cb(std::bind(&Map::_DestroyCubes, this, std::placeholders::_1, p.second));
                this->GetChunk(p.first, cb);
            }
            );
    }

    void Map::AddPlayer(std::shared_ptr<Player> const& p)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_AddPlayer, this, p));
        this->_messageQueue->PushMessage(m);
    }

    void Map::RemovePlayer(Uint32 id)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_RemovePlayer, this, id));
        this->_messageQueue->PushMessage(m);
    }

    void Map::_GetChunk(Chunk::IdType id, ChunkCallback& response)
    {
        Chunk* chunk = this->_chunkManager->GetChunk(id);
        if (chunk == 0)
        {
            if (this->_chunkRequests.find(id) == this->_chunkRequests.end())
            {
                this->_chunkRequests[id].push_back(response);
                Gen::ChunkGenerator::Callback cb(std::bind(&Map::HandleNewChunk, this, std::placeholders::_1));
                this->_gen->GetChunk(id, cb);
            }
            else
                this->_chunkRequests[id].push_back(response);
        }
        else
        {
            response(chunk);
        }
    }

    void Map::_SendChunkPacket(Chunk* chunk, ChunkPacketCallback& response)
    {
        auto toto = Network::PacketCreator::Chunk(*chunk);
        response(toto);
    }

    void Map::_HandleNewChunk(Chunk* chunk)
    {
        auto& requests = this->_chunkRequests[chunk->id];
        for (auto it = requests.begin(), ite = requests.end(); it != ite; ++it)
        {
            (*it)(chunk);
        }
        requests.clear();
        this->_chunkRequests.erase(chunk->id);

        this->_chunkManager->AddChunk(std::unique_ptr<Chunk>(chunk));
    }

    void Map::_GetSpawnPosition(SpawnCallback& response)
    {
        if (this->_spawnPosition != 0)
        {
            response(*this->_spawnPosition);
        }
        else
        {
            if (this->_spawnRequests.size() == 0)
            {
                this->_spawnRequests.push_back(response);
                ChunkCallback cb(std::bind(&Map::_FindSpawnPosition, this, std::placeholders::_1));
                this->GetChunk(Chunk::CoordsToId(1 << 21, (1 << 19) + 30, 1 << 21), cb);
            }
            else
                this->_spawnRequests.push_back(response);
        }
    }

    void Map::_FindSpawnPosition(Chunk* chunk)
    {
        for (int y = Common::ChunkSize - 1 ; y >= 0 ; --y)
        {
            if (chunk->GetCube(0, y, 0))
            {
                Common::CubeType const& biet = (*this->_conf.cubeTypes)[chunk->GetCube(0, y, 0) - 1];
                if (biet.solid)
                {
                    this->_spawnPosition = new Common::Position(chunk->coords, Tools::Vector3f(0, y, 0));
                    *this->_spawnPosition += Tools::Vector3f(0.5, 2.5, 0.5);
                    for (auto it = this->_spawnRequests.begin(), ite = this->_spawnRequests.end(); it != ite; ++it)
                        (*it)(*this->_spawnPosition);
                    this->_spawnRequests.clear();
                    return;
                }
                else
                {
                    for (unsigned int x = 0; x < Common::ChunkSize; ++x)
                    {
                        if (chunk->GetCube(x, y, 0))
                        {
                            Common::CubeType const& biet = (*this->_conf.cubeTypes)[chunk->GetCube(x, y, 0) - 1];
                            if (biet.solid)
                            {
                                this->_spawnPosition = new Common::Position(chunk->coords, Tools::Vector3f(x, y, 0));
                                *this->_spawnPosition += Tools::Vector3f(0.5, 2.5, 0.5);
                                for (auto it = this->_spawnRequests.begin(), ite = this->_spawnRequests.end(); it != ite; ++it)
                                    (*it)(*this->_spawnPosition);
                                this->_spawnRequests.clear();
                                return;
                            }
                        }
                    }
                    ChunkCallback cb(std::bind(&Map::_FindSpawnPosition, this, std::placeholders::_1));
                    this->GetChunk(Chunk::CoordsToId(chunk->coords.x + 1, chunk->coords.y, chunk->coords.z), cb);
                    return;
                }
            }
        }
        ChunkCallback cb(std::bind(&Map::_FindSpawnPosition, this, std::placeholders::_1));
        this->GetChunk(Chunk::CoordsToId(chunk->coords.x, chunk->coords.y - 1, chunk->coords.z), cb);
    }

    void Map::_AddPlayer(std::shared_ptr<Player> p)
    {
        this->_players[p->id] = p;
    }

    void Map::_RemovePlayer(Uint32 id)
    {
        this->_players.erase(id);
    }

    void Map::_DestroyCube(Chunk* chunk, Chunk::CoordsType cubePos)
    {
        if (chunk->GetCube(cubePos) != 0)
        {
            chunk->SetCube(cubePos, 0);
            this->_SendChunkToPlayers(chunk);
        }
    }

    void Map::_DestroyCubes(Chunk* chunk, std::vector<Chunk::CoordsType> cubePos)
    {
        bool send = false;

        std::for_each(cubePos.begin(), cubePos.end(), [chunk, &send](Chunk::CoordsType& pos)
            {
                if (chunk->GetCube(pos) != 0)
                {
                    chunk->SetCube(pos, 0);
                    send = true;
                }
            }
            );

        if (send)
            this->_SendChunkToPlayers(chunk);
    }

    void Map::_SendChunkToPlayers(Chunk* chunk)
    {
        for (auto it = this->_players.begin(), ite = this->_players.end(); it != ite; ++it)
        {
            auto toto = Network::PacketCreator::Chunk(*chunk);
            this->_game.GetServer().GetClientManager().SendPacket(it->first, toto);
        }
    }

    void Map::_Tick(Uint64 currentTime)
    {
        this->_engine->Tick(currentTime);
    }

}}}
