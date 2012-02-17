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
        Chunk& _chunk;
        std::map<int, std::pair<Tools::Renderers::IIndexBuffer*, Uint32>> _indices;
        Tools::Renderers::IVertexBuffer* _textureCoords;
        Uint32 _triangleCount;
        bool _hasTransparentCube;

    public:
        ChunkMesh(Chunk& chunk);
        ~ChunkMesh();

        void Refresh(Game::Game& game, ChunkRenderer& chunkRenderer);
        void Render(int cubeType, Tools::IRenderer& renderer);
        bool HasTransparentCube() const { return this->_hasTransparentCube; }
        bool HasCubeType(int cubeType) const
        {
            if (this->_triangleCount == 0)
                return false;
            auto it = this->_indices.find(cubeType);
            return (it->first != 0);
        }
    };

}}

#endif
