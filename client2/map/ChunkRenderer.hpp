#ifndef __CLIENT_MAP_CHUNKRENDERER_HPP__
#define __CLIENT_MAP_CHUNKRENDERER_HPP__

#include "common/BaseChunk.hpp"
#include "tools/IRenderer.hpp"

namespace Client {
    namespace Game {
        class CubeTypeManager;
        class Game;
    }
    namespace Map {
        class Chunk;
        class Map;
    }
}

namespace Client { namespace Map {

    class ChunkRenderer
        : private boost::noncopyable
    {
    public:
        struct CubeInfo
        {
            Tools::Renderers::ITexture2D* texture;
            Tools::Vector3f top; // x/u = position X, y/v = position Y, z/w = position Z
            Tools::Vector3f bottom;
            Tools::Vector3f front;
            Tools::Vector3f back;
            Tools::Vector3f left;
            Tools::Vector3f right;
            bool isTransparent;
        };

    private:
        Game::Game& _game;
        Tools::IRenderer& _renderer;
        Tools::Renderers::IShaderProgram* _shader;
        Tools::Renderers::IShaderParameter* _shaderTexture;
        Tools::Renderers::IVertexBuffer* _baseVbo;
        std::vector<CubeInfo> _cubeInfo;

    public:
        ChunkRenderer(Game::Game& game);
        ~ChunkRenderer();

        void RefreshDisplay(Chunk& chunk);
        void Render();
        CubeInfo const& GetCubeInfo(int cubeType) const { return this->_cubeInfo[cubeType - 1]; }
    private:
        void _InitBaseVbo();
    };

}}

#endif
