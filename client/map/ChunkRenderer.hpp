#ifndef __CLIENT_MAP_CHUNKRENDERER_HPP__
#define __CLIENT_MAP_CHUNKRENDERER_HPP__

#include "common/BaseChunk.hpp"
#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/utils/DeferredShading.hpp"
#include "tools/gfx/utils/material/LuaMaterial.hpp"
#include "tools/gfx/utils/texture/ITexture.hpp"

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
}

namespace Client { namespace Map {

    class ChunkRenderer
        : private boost::noncopyable
    {
    private:
        Game::Game& _game;
        Tools::Gfx::IRenderer& _renderer;
        std::map<Tools::Gfx::Utils::Material::Material*, std::multimap<double, Chunk*>> _transparentChunks;

    public:
        ChunkRenderer(Game::Game& game);
        ~ChunkRenderer();

        bool RefreshGraphics(Chunk& chunk);
        void Render(Tools::Gfx::Utils::DeferredShading& deferredShading, Common::Position const& position, glm::dmat4 const& viewProj);
    };

}}

#endif
