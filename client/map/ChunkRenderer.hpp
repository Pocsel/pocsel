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
    namespace Resources {
        class ResourceManager;
    }
}

namespace Client { namespace Map {

    class ChunkRenderer
        : private boost::noncopyable
    {
    private:
        struct Effect
        {
            Tools::Renderers::IShaderProgram* shader;
            Tools::Renderers::IShaderParameter* shaderTexture;
            Tools::Renderers::IShaderParameter* shaderNormalMap;
            Tools::Renderers::IShaderParameter* shaderTime;

            Effect(Resources::ResourceManager& resourceManager, Uint32 id);
            Effect(Tools::Renderers::IShaderProgram* shader, std::string const& texture, std::string const& normalMap, std::string const& time);
            Effect(Effect&& effect);
            ~Effect();
        private:
            Effect(Effect const&);
            Effect& operator =(Effect const&);
        };

        Tools::Timer _tmpTimer; // TODO: gerer le temps cote serveur et l'envoyer au client
        Game::Game& _game;
        Tools::IRenderer& _renderer;
        std::map<Uint32, Effect> _effects;
        std::map<Uint32, Tools::Renderers::ITexture2D*> _textures;


    public:
        ChunkRenderer(Game::Game& game);
        ~ChunkRenderer();

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
