#ifndef __CLIENT_MAP_CHUNKMESH_HPP__
#define __CLIENT_MAP_CHUNKMESH_HPP__

#include "tools/IRenderer.hpp"

namespace Common {
    struct Camera;
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
            Tools::Renderers::IVertexBuffer* vertices;
            Uint32 nbIndices;
            
            Mesh() : indices(0), vertices(0), nbIndices(0) {}
            Mesh(Mesh&& m);
            ~Mesh();

            Mesh& operator =(Mesh&& m);
        };
        Chunk& _chunk;
        std::map<int, Mesh> _meshes;
        Uint32 _triangleCount;
        bool _hasTransparentCube;

    public:
        ChunkMesh(Chunk& chunk);
        ~ChunkMesh();

        void Refresh(Game::Game& game, ChunkRenderer& chunkRenderer);
        void Render(int cubeType, Tools::IRenderer& renderer);
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
