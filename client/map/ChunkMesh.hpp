#ifndef __CLIENT_MAP_CHUNKMESH_HPP__
#define __CLIENT_MAP_CHUNKMESH_HPP__

#include "tools/IRenderer.hpp"

#include "common/BaseChunk.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {
    class Material;
}}}}

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
            Tools::Renderers::IIndexBuffer* indices;
            Uint32 nbIndices;

            Mesh() : indices(0), nbIndices(0) {}
            Mesh(Mesh&& m);
            ~Mesh();

            Mesh& operator =(Mesh&& m);
        };
        Chunk& _chunk;
        Tools::Renderers::IVertexBuffer* _vertices;
        std::map<Tools::Renderers::Utils::Material::Material*, Mesh> _meshes;
        Uint32 _triangleCount;
        bool _hasTransparentCube;
        bool _isComputed;
        boost::mutex _refreshMutex;
        unsigned int _tmpNbVertices;
        float* _tmpVertices;
        std::map<Tools::Renderers::Utils::Material::Material*, std::vector<unsigned int>> _tmpIndices;

    public:
        ChunkMesh(Chunk& chunk);
        ~ChunkMesh();

        bool Refresh(ChunkRenderer& chunkRenderer,
                     std::vector<Game::CubeType> const& cubeTypes,
                     std::shared_ptr<Common::BaseChunk::CubeType> cubes,
                     std::vector<std::shared_ptr<Common::BaseChunk::CubeType>> neighbors);

        bool RefreshGraphics(Tools::IRenderer& renderer);
        void Render(Tools::Renderers::Utils::Material::Material& material, Tools::IRenderer& renderer);
        Uint32 GetTriangleCount() const { return this->_triangleCount; }
        Uint32 GetTriangleCount(Tools::Renderers::Utils::Material::Material& material) const
        {
            if (this->_triangleCount == 0)
                return 0;
            auto it = this->_meshes.find(&material);
            if (it == this->_meshes.end())
                return 0;
            return it->second.nbIndices * 4 / 3;
        }
        bool HasTransparentCube() const { return this->_hasTransparentCube; }
    };

}}

#endif
