#ifndef __CLIENT_MAP_CHUNKMESH_HPP__
#define __CLIENT_MAP_CHUNKMESH_HPP__

#include "tools/gfx/IRenderer.hpp"

#include "common/BaseChunk.hpp"

namespace Tools { namespace Gfx { namespace Utils {
    class DeferredShading;
    namespace Material {
        class Material;
    }
}}}

namespace Client {
    namespace Game {
        class CubeType;
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
            Tools::Gfx::IIndexBuffer* indices;
            Uint32 nbIndices;

            Mesh() : indices(0), nbIndices(0) {}
            Mesh(Mesh&& m);
            ~Mesh();

            Mesh& operator =(Mesh&& m);
        };
        Chunk& _chunk;
        Tools::Gfx::IVertexBuffer* _vertices;
        std::map<Tools::Gfx::Utils::Material::Material*, Mesh> _meshes;
        Uint32 _triangleCount;
        bool _hasTransparentCube;
        bool _isComputed;
        boost::mutex _refreshMutex;
        unsigned int _tmpNbVertices;
        float* _tmpVertices;
        std::map<Tools::Gfx::Utils::Material::Material*, std::vector<unsigned int>> _tmpIndices;

    public:
        ChunkMesh(Chunk& chunk);
        ~ChunkMesh();

        bool Refresh(ChunkRenderer& chunkRenderer,
                     std::vector<Game::CubeType> const& cubeTypes,
                     std::shared_ptr<Common::BaseChunk::CubeArray> cubes,
                     std::vector<std::shared_ptr<Common::BaseChunk::CubeArray>> neighbors);

        bool RefreshGraphics(Tools::Gfx::IRenderer& renderer);
        void Render(Tools::Gfx::IRenderer& renderer, Tools::Gfx::Utils::DeferredShading& deferredShading, glm::mat4 const& world, Uint32 squaredDistance);
        Uint32 GetTriangleCount() const { return this->_triangleCount; }
        bool HasTransparentCube() const { return this->_hasTransparentCube; }
    };

}}

#endif
