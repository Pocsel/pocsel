# include "server/precompiled.hpp"

#include "server/game/map/Map.hpp"
#include "server/game/map/ChunkManager.hpp"

#include "common/CubeType.hpp"
#include "common/Position.hpp"
#include "common/MovingOrientedPosition.hpp"
#include "common/CubePosition.hpp"
#include "common/Packet.hpp"
#include "common/RayCast.hpp"

#include "tools/SimpleMessageQueue.hpp"

#include "tools/database/IConnection.hpp"
#include "tools/database/sqlite/Connection.hpp"

#include "server/constants.hpp"
#include "server/Server.hpp"
#include "server/clientmanagement/ClientManager.hpp"
#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "server/game/Player.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/EntityManager.hpp"
#include "server/game/map/gen/ChunkGenerator.hpp"
#include "server/database/ResourceManager.hpp"
#include "server/network/PacketCreator.hpp"
#include "server/network/UdpPacket.hpp"

namespace Server { namespace Game { namespace Map {

    Map::Map(Conf const& conf,
            Uint64 currentTime,
            World& world,
            std::vector<Chunk::IdType> const& existingBigChunks) :
        _conf(conf),
        _world(world),
        _game(world.GetGame()),
        _messageQueue(new Tools::SimpleMessageQueue(1)),
        _spawnPosition(0),
        _currentTime(currentTime)
    {
        Tools::debug << "Map::Map() -- " << this->_conf.name << "\n";
        this->_gen = new Gen::ChunkGenerator();
        this->_engine = new Engine::Engine(*this, this->_world);
        this->_chunkManager = new ChunkManager(*this, this->_game.GetServer().GetResourceManager().GetConnection(), existingBigChunks);
    }

    Map::~Map()
    {
        this->_conf.cubes.clear();
        this->_conf.cubeTypes.clear();
        Tools::debug << "Map::~Map() -- " << this->_conf.name << "\n";
        Tools::Delete(this->_gen);
        Tools::Delete(this->_engine);
        Tools::Delete(this->_messageQueue);
        Tools::Delete(this->_chunkManager);
        Tools::Delete(this->_spawnPosition);
    }

    void Map::Start()
    {
        Tools::debug << "Map::Start() -- " << this->_conf.name << "\n";
        this->_gen->Start(this->_conf);
        this->_messageQueue->Start();

        auto& plugins = this->_game.GetWorld().GetPluginManager().GetPlugins();
        auto it = plugins.begin();
        auto itEnd = plugins.end();
        for (; it != itEnd; ++it)
            this->_engine->GetEntityManager().BootstrapPlugin(it->first);

        // tick toutes les 10 ms
        Tools::SimpleMessageQueue::TimerLoopMessage
            tlm(std::bind(&Map::_Tick, this, std::placeholders::_1));
        this->_messageQueue->SetLoopTimer(10000, tlm, this->_currentTime);

        // sauvegarde
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_TimedSave, this));
        this->_messageQueue->PushTimedMessage(SaveTime * 1000000, m);
    }

    void Map::Stop()
    {
        Tools::debug << "Map::Stop() -- " << this->_conf.name << "\n";
        this->_gen->Stop();
        this->_messageQueue->Stop();
    }

    void Map::Save()
    {
        std::cout << "MapSave\n";

        auto& conn = this->_game.GetServer().GetResourceManager().GetConnection();
        conn.CreateQuery("UPDATE map SET tick = ? WHERE name = ?")->Bind(this->_currentTime).Bind(this->_conf.name).ExecuteNonSelect();

        this->_chunkManager->Save(conn);
        this->_engine->Save(conn);
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
        ChunkCallback cb(std::bind(&Map::_DestroyCube, this, std::placeholders::_1, Common::GetCubeCoordsInChunk(pos)));
        this->GetChunk(Chunk::CoordsToId(Common::GetChunkCoords(pos)), cb);
    }

    void Map::DestroyCubes(std::vector<Common::CastChunk*> const& pos)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_PreDestroyCubes,
                        this,
                        std::shared_ptr<std::vector<Common::CastChunk*>>(
                            new std::vector<Common::CastChunk*>(pos))
                        ));
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

    void Map::MovePlayer(Uint32 id, Common::MovingOrientedPosition const& pos)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_MovePlayer, this, id, pos));
        this->_messageQueue->PushMessage(m);
    }

    void Map::_GetChunk(Chunk::IdType id, ChunkCallback& response)
    {
        Chunk* chunk = this->_chunkManager->GetChunk(id);
        if (chunk == 0)
        {
            if (this->_chunkRequests.find(id) == this->_chunkRequests.end())
            {
                Gen::ChunkGenerator::Callback cb(std::bind(&Map::HandleNewChunk, this, std::placeholders::_1));
                this->_gen->GetChunk(id, cb);
            }
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
            if (*it != 0)
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
        // les limites du monde lol
//        this->_spawnPosition = new Common::Position(1000.0);
//        *this->_spawnPosition += glm::dvec3(0.5, 2.5, 0.5);
//        for (auto it = this->_spawnRequests.begin(), ite = this->_spawnRequests.end(); it != ite; ++it)
//            (*it)(*this->_spawnPosition);
//        this->_spawnRequests.clear();
//        return;

        for (int y = Common::ChunkSize - 1 ; y >= 0 ; --y)
        {
            if (chunk->GetCube(0, y, 0))
            {
                CubeType const& biet = this->_conf.cubeTypes[chunk->GetCube(0, y, 0) - 1];
                if (biet.solid)
                {
                    this->_spawnPosition = new Common::Position(Common::GetChunkPosition(chunk->coords) + glm::dvec3(0, (float)y, 0));
                    *this->_spawnPosition += glm::dvec3(0.5, 4.5, 0.5);
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
                            CubeType const& biet = this->_conf.cubeTypes[chunk->GetCube(x, y, 0) - 1];
                            if (biet.solid)
                            {
                                this->_spawnPosition = new Common::Position(Common::GetChunkPosition(chunk->coords) + glm::dvec3(x, y, 0));
                                *this->_spawnPosition += glm::dvec3(0.5f, 4.5f, 0.5f);
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

    void Map::_MovePlayer(Uint32 id, Common::MovingOrientedPosition pos)
    {
        auto it = this->_players.find(id);
        if (it == this->_players.end())
            return;

        this->_players[id]->SetPosition(pos);

        for (auto it = this->_players.begin(), ite = this->_players.end(); it != ite; ++it)
        {
            if (id == it->first)
                continue;
            auto toto = Network::PacketCreator::ItemMove(pos, id);
            this->_game.GetServer().GetClientManager().SendUdpPacket(it->first, toto);
        }
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

    void Map::_DestroyCubes2(Chunk* chunk, std::shared_ptr<Common::CastChunk> pos)
    {
        bool send = false;

        auto cubePos = pos->GetCubes();

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

    void Map::_DestroyChunkCallback(Chunk* chunk)
    {
        chunk->StealCubes();
        this->_SendChunkToPlayers(chunk);
    }

    void Map::_DestroyChunk(Chunk::IdType id)
    {
        Chunk* chunk = this->_chunkManager->GetChunk(id);
        if (chunk == 0)
        {
            if (this->_chunkRequests.count(id) == 0)
            {
                chunk = new Chunk(id);
                this->_SendChunkToPlayers(chunk);
                this->_chunkManager->AddChunk(std::unique_ptr<Chunk>(chunk));
            }
            else
            {
                ChunkCallback cb(std::bind(&Map::_DestroyChunkCallback, this, std::placeholders::_1));
                this->_chunkRequests[id].push_back(cb);
            }
        }
        else
        {
            chunk->StealCubes();
            this->_SendChunkToPlayers(chunk);
        }
    }

    void Map::_PreDestroyCubes(std::shared_ptr<std::vector<Common::CastChunk*>> pos)
    {
        if (pos->empty() == true)
            return;

        Common::CastChunk* c = pos->back();
        pos->pop_back();

        while (c && c->full == true)
        {
            this->_DestroyChunk(c->id);
            Tools::Delete(c);
            c = 0;

            if (!pos->empty())
            {
                c = pos->back();
                pos->pop_back();
            }
        }

        if (c != 0)
        {
            ChunkCallback
                cb(std::bind(&Map::_DestroyCubes2, this, std::placeholders::_1, std::shared_ptr<Common::CastChunk>(c)));
            this->GetChunk(c->id, cb);
        }

        if (pos->empty() == false)
        {
            Tools::SimpleMessageQueue::Message
                m(std::bind(&Map::_PreDestroyCubes, this, pos));
            //this->_messageQueue->PushMessage(m);
            this->_messageQueue->PushTimedMessage(0, m);
        }
    }

    void Map::_SendChunkToPlayers(Chunk* chunk)
    {
        auto packet = Network::PacketCreator::Chunk(*chunk);
        for (auto it = this->_players.begin(), ite = this->_players.end(); it != ite; ++it)
        {
            Player* p = it->second.get();
            int viewDist = p->GetViewDistance();
            Common::Position const& playerPos = p->GetPosition().position.position;
            if (viewDist * Common::ChunkSize >= std::abs(playerPos.x - (int)(chunk->coords.x * Common::ChunkSize)) ||
                viewDist * Common::ChunkSize >= std::abs(playerPos.y - (int)(chunk->coords.y * Common::ChunkSize)) ||
                viewDist * Common::ChunkSize >= std::abs(playerPos.z - (int)(chunk->coords.z * Common::ChunkSize)))
            {
                auto toto = std::unique_ptr<Common::Packet>(new Common::Packet(*packet));
                this->_game.GetServer().GetClientManager().SendPacket(it->first, toto);
            }
        }
    }

    void Map::_Tick(Uint64 currentTime)
    {
        this->_currentTime = currentTime;
        this->_engine->Tick(currentTime);
    }

    void Map::_TimedSave()
    {
        boost::mutex& mutex = this->_game.GetWorld().saveMutex;
        if (!mutex.try_lock())
        {
            // on reessaie dans 7 secondes
            Tools::log << this->GetName() << ": could not save, retry in 7 seconds.\n";
            Tools::SimpleMessageQueue::Message
                m(std::bind(&Map::_TimedSave, this));
            this->_messageQueue->PushTimedMessage(SaveRetryTime * 1000000, m);
            return;
        }
        boost::lock_guard<boost::mutex> lock(mutex, boost::adopt_lock);
        Tools::log << this->GetName() << ": Saving.\n";
        this->Save();

        // resave
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_TimedSave, this));
        this->_messageQueue->PushTimedMessage(SaveTime * 1000000, m);
    }

    void Map::RconExecute(Uint32 pluginId, std::string lua, std::function<void(std::string)> cb) const
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_RconExecute, this, pluginId, lua, cb));
        this->_messageQueue->PushMessage(m);
    }

    void Map::_RconExecute(Uint32 pluginId, std::string const& lua, std::function<void(std::string)> cb) const
    {
        cb(this->_engine->GetEntityManager().RconExecute(pluginId, lua));
    }

    void Map::RconGetEntities(std::function<void(std::string)> cb) const
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&Map::_RconGetEntities, this, cb));
        this->_messageQueue->PushMessage(m);
    }

    void Map::_RconGetEntities(std::function<void(std::string)> cb) const
    {
        cb(this->_engine->GetEntityManager().RconGetEntities());
    }

}}}
