#ifndef __CLIENT_MAP_CHUNKMESH_HPP__
#define __CLIENT_MAP_CHUNKMESH_HPP__

#include "tools/IRenderer.hpp"

#include "common/BaseChunk.hpp"

namespace Common {
    struct Camera;
    struct CubeType;
}

namespace Client {
    namespace Game {
        class Game;
    }
    namespace Map {
        class Chunk;
        class ChunkRenderer;
    }
}

namespace Client { namespace Map {

    class ChunkMesh
        : private boost::noncopyable
    {
    private:
        struct Mesh : private boost::noncopyable
        {
            Tools::Renderers::IIndexBuffer* indices;
            Uint32 nbIndices;

            Mesh() : indices(0), nbIndices(0) {}
            Mesh(Mesh&& m);
            ~Mesh();

            Mesh& operator =(Mesh&& m);
        };
        Chunk& _chunk;
        Tools::Renderers::IVertexBuffer* _vertices;
        std::map<Uint32, Mesh> _meshes;
        Uint32 _triangleCount;
        bool _hasTransparentCube;
        bool _isComputed;
        boost::mutex _refreshMutex;
        unsigned int _tmpNbVertices;
        float* _tmpVertices;
        std::map<Uint32, std::vector<unsigned int>> _tmpIndices;

    public:
        ChunkMesh(Chunk& chunk);
        ~ChunkMesh();

        bool Refresh(ChunkRenderer& chunkRenderer,
                     std::vector<Common::CubeType> cubeTypes,
                     std::shared_ptr<Common::BaseChunk::CubeType> cubes,
                     std::vector<std::shared_ptr<Common::BaseChunk::CubeType>> neighbors);

        bool RefreshGraphics(Tools::IRenderer& renderer);
        void Render(Uint32 textureId, Tools::IRenderer& renderer);
        Uint32 GetTriangleCount() const { return this->_triangleCount; }
        bool HasTransparentCube() const { return this->_hasTransparentCube; }
        bool HasCubeType(int cubeType) const
        {
            if (this->_triangleCount == 0)
                return false;
            auto it = this->_meshes.find(cubeType);
            return (it->second.nbIndices > 0);
        }
    };

}}

#endif
