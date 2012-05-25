#ifndef __CLIENT_MAP_CHUNKRENDERER_HPP__
#define __CLIENT_MAP_CHUNKRENDERER_HPP__

#include "common/BaseChunk.hpp"
#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

namespace Client {
    namespace Game {
        class CubeType;
        class CubeTypeManager;
        class Game;
    }
    namespace Map {
        class Chunk;
        class Map;
    }
    namespace Resources {
        class Effect;
    }
}

namespace Client { namespace Map {

    class ChunkRenderer
        : private boost::noncopyable
    {
    private:
        Game::Game& _game;
        Tools::IRenderer& _renderer;
        std::map<Uint32, std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture>> _textures;
        std::map<Resources::Effect*,
            std::pair<
                std::unique_ptr<Tools::Renderers::IShaderParameter>,
                std::map<Uint32, Tools::Renderers::Utils::Texture::ITexture*>
            >> _cubeTypes;
        std::map<Uint32, std::multimap<double, Chunk*>> _transparentChunks;

    public:
        ChunkRenderer(Game::Game& game);
        ~ChunkRenderer();

        bool RefreshGraphics(Chunk& chunk);
        void Update(Uint64 totalTime);
        void Render(Common::Position const& position, glm::dmat4 viewProj);
        void RenderAlpha(Common::Position const& position);
        Tools::Renderers::ITexture2D& GetTexture(Uint32 id)
        {
            auto it = this->_textures.find(id);
            if (it == this->_textures.end())
                throw std::runtime_error("bad id");
            return it->second->GetCurrentTexture();
        }
        std::map<Uint32, std::unique_ptr<Tools::Renderers::Utils::Texture::ITexture>> const& GetAllTextures() const { return this->_textures; }
    };

}}

#endif
