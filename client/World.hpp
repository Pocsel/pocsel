#ifndef __CLIENT_WORLD_HPP__
#define __CLIENT_WORLD_HPP__

#include <deque>
#include <list>
#include <unordered_map>
#include <boost/noncopyable.hpp>

#include "client/Chunk.hpp"
#include "client/Octree.hpp"

namespace Tools {
    class IRenderer;
    namespace Gui {
        class EventManager;
        struct Event;
    }
}

namespace Common {
    struct Camera;
}

namespace Client
{

    class Player;
    class ResourceManager;
    class CacheDatabaseProxy;
    class Network;
    class PacketDispatcher;

    class World :
        private boost::noncopyable
    {
        private:
            typedef std::unordered_map<Chunk::IdType, Chunk*> ChunkMap;
        private:
            Player& _player;
            Tools::IRenderer& _renderer;
            Octree _octree;
            ChunkMap _chunks;
            std::list<Chunk*> _pendingDisplayChunk;

        public:
            World(Tools::Gui::EventManager& eventManager,
                    Network& network,
                    Tools::IRenderer& renderer,
                    PacketDispatcher& packetDispatcher,
                    Player& player);
            ~World();
            /**
             * Returns 0 if the chunk is not loaded
             */
            Chunk const* GetChunk(Chunk::IdType id) const;
            Chunk const* GetChunk(Common::BaseChunk::CoordsType const& chunkPosition) const;
            Chunk* GetChunk(Chunk::IdType id);
            Chunk* GetChunk(Common::BaseChunk::CoordsType const& chunkPosition);
            Common::BaseChunk::CubeType GetCube(Common::BaseChunk::CoordsType const& chunkPosition, Tools::Vector3<Uint32> const& cubePosition) const;
            void InsertChunk(std::unique_ptr<Chunk>& newChunk);
            void InvalidateChunk(Chunk& chunk);
            void RenderChunks();
            std::deque<Chunk::IdType> GetNeededChunks();
        private:
            void _OnPlayerMoved(Tools::Gui::Event const&);
    };

}

#endif
