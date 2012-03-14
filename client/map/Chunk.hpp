#ifndef __CLIENT_MAP_CHUNK_HPP__
#define __CLIENT_MAP_CHUNK_HPP__

#include "common/BaseChunk.hpp"
#include "client/map/ChunkMesh.hpp"

namespace Common {
    struct Camera;
}
namespace Client { namespace Map {
    class Map;
}}

namespace Client { namespace Map {

    class Chunk
        : public Common::BaseChunk,
        private boost::noncopyable
    {
    private:
        ChunkMesh* _mesh;

    public:
        explicit Chunk(IdType id);
        explicit Chunk(CoordsType const& c);
        ~Chunk();

        ChunkMesh* GetMesh() { return this->_mesh; }
        void SetMesh(std::unique_ptr<ChunkMesh> mesh);
    };

}}

#endif
