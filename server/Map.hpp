#ifndef __SERVER_MAP_HPP__
#define __SERVER_MAP_HPP__

#include <list>
#include <memory>
#include <unordered_map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include "tools/database/IConnectionPool.hpp"
#include "tools/MessageQueue.hpp"

#include "common/CubeType.hpp"

#include "server/ClientPtr.hpp"
#include "server/Chunk.hpp"
#include "server/mapgen/MapGenerator.hpp"

namespace Common {

    struct Position;

}

namespace Server {

    struct MapConf
    {
        std::string name;
        std::string fullname;
        bool is_default;
        struct EquationConf
        {
            std::string function_name;
            std::map<std::string, double> values;
        };
        std::map<std::string, EquationConf> equations;
        struct ValidatorConf
        {
            std::string equation;
            std::string validator;
            std::map<std::string, double> values;
        };
        struct ValidationBlocConf
        {
            int priority;
            Common::CubeType* cube_type; // ça se répète avec celui de CubeConf mais c'est fait exprès
            std::list<ValidatorConf> validators;
        };
        struct CubeConf
        {
            Common::CubeType* type;
            std::list<ValidationBlocConf> validation_blocs;
        };
        std::map<std::string, CubeConf> cubes;
        std::vector<Common::CubeType> const* cubeTypes;
    };

    class Map :
        public Tools::MessageQueue<Map>,
        private boost::noncopyable
    {
    public:
        typedef boost::function<void(Chunk const&)> RequestCallback;
        typedef boost::function<void(Common::Position const& pos)> SpawnPositionCallback;

    private:
        typedef std::unordered_map<Chunk::IdType, std::list<RequestCallback>> RequestsMap;
        typedef std::list<SpawnPositionCallback> SpawnRequestList;

    private:
        MapConf                                     _conf;
        MapGen::MapGenerator                        _mapGen;
        std::unordered_map<Chunk::IdType, Chunk*>   _chunks;
        RequestsMap                                 _requests;
        SpawnRequestList                            _spawnRequestList;
        Common::Position*                           _spawnPosition;

    public:
        Map(MapConf const&);
        ~Map();

        void GetChunk(Chunk::IdType id, RequestCallback response)
        {
            this->PushCall(std::bind(&Map::_GetChunk, this, id, response));
        }

        void GetSpawnPosition(SpawnPositionCallback response);


        TOOLS_MQ_WRAP2(void, Map, OnNewChunk, Chunk::IdType, id, Chunk*, chunk)
        {
            this->_chunks[id] = chunk;
            auto& requests = this->_requests[id];
            auto it = requests.begin(), end = requests.end();
            for (; it != end; ++it)
            {
                (*it)(*chunk);
            }
            requests.clear();
        }

    private:
        void _GetChunk(Chunk::IdType id, RequestCallback response)
        {
            auto chunk_it = this->_chunks.find(id);
            if (chunk_it == this->_chunks.end())
            {
                this->_requests[id].push_back(response);
                this->_mapGen.GetChunk(id, std::bind(&Map::OnNewChunk, this, id, std::placeholders::_1));
            }
            else
            {
                Chunk* chunk = chunk_it->second;
                response(*chunk);
            }
        }
        void _GetSpawnPosition(Chunk const& chunk);
    };

}

#endif
