#ifndef __CLIENT_MAP_CHUNKRENDERER_HPP__
#define __CLIENT_MAP_CHUNKRENDERER_HPP__

#include "common/BaseChunk.hpp"
#include "common/CubeType.hpp"
#include "tools/IRenderer.hpp"
#include "client/resources/ITexture.hpp"

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
        std::map<Uint32, std::unique_ptr<Resources::ITexture>> _textures;
        //std::map<Uint32, Effect*> _effects;

    public:
        ChunkRenderer(Game::Game& game);
        ~ChunkRenderer();

        bool RefreshGraphics(Chunk& chunk);
        void Update(Uint64 totalTime);
        void Render();
        Tools::Renderers::ITexture2D& GetTexture(Uint32 id)
        {
            auto it = this->_textures.find(id);
            if (it == this->_textures.end())
                throw std::runtime_error("bad id");
            return it->second->GetCurrentTexture();
        }
    };

}}

#endif
