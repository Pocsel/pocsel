#ifndef __CLIENT_MAP_CHUNKRENDERER_HPP__
#define __CLIENT_MAP_CHUNKRENDERER_HPP__

#include "common/BaseChunk.hpp"
#include "common/CubeType.hpp"
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
    private:
        Game::Game& _game;
        Tools::IRenderer& _renderer;
        Tools::Renderers::IShaderProgram* _shader;
        Tools::Renderers::IShaderParameter* _shaderTexture;
        std::map<Uint32, Tools::Renderers::ITexture2D*> _textures;


    public:
        ChunkRenderer(Game::Game& game);
        ~ChunkRenderer();

        bool RefreshMesh(Chunk& chunk);
        bool RefreshGraphics(Chunk& chunk);
        void Render();
        Tools::Renderers::ITexture2D& GetTexture(Uint32 id)
        {
            auto it = this->_textures.find(id);
            if (it == this->_textures.end())
                throw std::runtime_error("bad id");
            return *it->second;
        }
    };

}}

#endif
