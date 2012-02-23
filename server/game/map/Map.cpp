#include "server/game/map/Map.hpp"

#include "common/CubeType.hpp"
#include "common/Position.hpp"
#include "common/CubePosition.hpp"

#include "tools/SimpleMessageQueue.hpp"

#include "server/game/map/gen/ChunkGenerator.hpp"

#include "server/game/entities/EntityManager.hpp"
#include "server/game/engine/Engine.hpp"

#include "server/game/Game.hpp"
#include "server/game/Player.hpp"

#include "server/Server.hpp"
#include "server/clientmanagement/ClientManager.hpp"

namespace Server { namespace Game { namespace Map {

    Map::Map(Conf const& conf, Game& game) :
        _conf(conf),
        _game(game),
        _messageQueue(new Tools::SimpleMessageQueue(1)),
        _spawnPosition(0)
    {
        Tools::debug << "Map::Map() -- " << this->_conf.name << "\n";
        this->_gen = new Gen::ChunkGenerator(this->_conf);
        this->_engine = new Engine::Engine(*this->_messageQueue);
        this->_entityManager = new Entities::EntityManager();
    }

    Map::~Map()
    {
        Tools::debug << "Map::~Map() -- " << this->_conf.name << "\n";
        for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
        Tools::Delete(this->_gen);
        Tools::Delete(this->_engine);
        Tools::Delete(this->_entityManager);
        Tools::Delete(this->_messageQueue);
    }

    void Map::Start()
    {
        Tools::debug << "Map::Start() -- " << this->_conf.name << "\n";
        this->_gen->Start();
        this->_entityManager->Start();
        this->_messageQueue->Start();
    }

    void Map::Stop()
    {
        Tools::debug << "Map::Stop() -- " << this->_conf.name << "\n";
        this->_gen->Stop();
        this->_entityManager->Stop();
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
        Tools::debug << "GetSpawnPosition\n";
    }

    void Map::GetChunk(Chunk::IdType id, ChunkCallback& response)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_GetChunk, this, id, response));
        this->_messageQueue->PushMessage(m);
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

    void Map::DestroyCube(Common::CubePosition const& pos)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_DestroyCube, this, pos));
        this->_messageQueue->PushMessage(m);
    }

    void Map::_GetChunk(Chunk::IdType id, ChunkCallback& response)
    {
        auto chunk_it = this->_chunks.find(id);
        if (chunk_it == this->_chunks.end())
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
            response(*chunk_it->second);
        }
    }

    void Map::_HandleNewChunk(Chunk* chunk)
    {
        assert(this->_chunks.find(chunk->id) == this->_chunks.end() && "CHUNK ALREADY GENERATED");
        this->_chunks[chunk->id] = chunk;
        auto& requests = this->_chunkRequests[chunk->id];
        for (auto it = requests.begin(), ite = requests.end(); it != ite; ++it)
        {
            (*it)(*chunk);
        }
        requests.clear();
        this->_chunkRequests.erase(chunk->id);
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

    void Map::_FindSpawnPosition(Chunk const& chunk)
    {
        Tools::debug << "_FindSpawnPosition\n";
        for (int y = Common::ChunkSize - 1 ; y >= 0 ; --y)
        {
            if (chunk.GetCube(0, y, 0))
            {
                Common::CubeType const& biet = (*this->_conf.cubeTypes)[chunk.GetCube(0, y, 0) - 1];
                if (biet.solid)
                {
                    this->_spawnPosition = new Common::Position(chunk.coords, Tools::Vector3f(0, y, 0));
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
                        if (chunk.GetCube(x, y, 0))
                        {
                            Common::CubeType const& biet = (*this->_conf.cubeTypes)[chunk.GetCube(x, y, 0) - 1];
                            if (biet.solid)
                            {
                                this->_spawnPosition = new Common::Position(chunk.coords, Tools::Vector3f(x, y, 0));
                                *this->_spawnPosition += Tools::Vector3f(0.5, 2.5, 0.5);
                                for (auto it = this->_spawnRequests.begin(), ite = this->_spawnRequests.end(); it != ite; ++it)
                                    (*it)(*this->_spawnPosition);
                                this->_spawnRequests.clear();
                                return;
                            }
                        }
                    }
                    ChunkCallback cb(std::bind(&Map::_FindSpawnPosition, this, std::placeholders::_1));
                    this->GetChunk(Chunk::CoordsToId(chunk.coords.x + 1, chunk.coords.y, chunk.coords.z), cb);
                    return;
                }
            }
        }
        ChunkCallback cb(std::bind(&Map::_FindSpawnPosition, this, std::placeholders::_1));
        this->GetChunk(Chunk::CoordsToId(chunk.coords.x, chunk.coords.y - 1, chunk.coords.z), cb);
    }

    void Map::_AddPlayer(std::shared_ptr<Player> p)
    {
        this->_players[p->id] = p;
    }

    void Map::_RemovePlayer(Uint32 id)
    {
        this->_players.erase(id);
    }

    void Map::_DestroyCube(Common::CubePosition pos)
    {
        Chunk::IdType id = Chunk::CoordsToId(pos.world);

        auto it = this->_chunks.find(id);
        if (it == this->_chunks.end())
            return;
        Chunk* chunk = it->second;

        if (chunk->GetCube(pos.chunk) != 0)
        {
            chunk->SetCube(pos.chunk, 0);

            for (auto it = this->_players.begin(), ite = this->_players.end(); it != ite; ++it)
            {
                this->_game.GetServer().GetClientManager().SendChunk(it->second->id, *chunk);
            }
        }
    }

}}}
