#include "client2/precompiled.hpp"

#include "client2/map/Chunk.hpp"

namespace Client { namespace Map {

    Chunk::Chunk(IdType id)
        : Common::BaseChunk(id),
        _mesh(0)
    {
    }

    Chunk::Chunk(CoordsType const& c)
        : Common::BaseChunk(c)
    {
    }

    Chunk::~Chunk()
    {
        Tools::Delete(this->_mesh);
    }

    void Chunk::SetMesh(std::unique_ptr<ChunkMesh> mesh)
    {
        if (this->_mesh != 0)
            delete this->_mesh;
        this->_mesh = mesh.release();
    }
}}