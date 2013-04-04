#include "client/precompiled.hpp"

#include <luasel/Luasel.hpp>

#include "tools/window/Window.hpp"
#include "tools/Frustum.hpp"

#include "client/Client.hpp"
#include "client/game/CubeType.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/ChunkRenderer.hpp"
#include "client/map/Map.hpp"
#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Map {

    ChunkRenderer::ChunkRenderer(Game::Game& game)
        : _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer())
    {
    }

    ChunkRenderer::~ChunkRenderer()
    {
    }

    bool ChunkRenderer::RefreshGraphics(Chunk& chunk)
    {
        return chunk.GetMesh()->RefreshGraphics(this->_renderer);
    }

    void ChunkRenderer::Render(Tools::Gfx::Utils::DeferredShading& deferredShading, Common::Position const& position, glm::dmat4 const& viewProj)
    {
        this->_game.GetMap().GetChunkManager().ForeachIn(Tools::Frustum(viewProj),
            [&](Chunk& chunk)
            {
                auto mesh = chunk.GetMesh();
                if (mesh != 0 && mesh->GetTriangleCount() != 0)
                {
                    auto deltaPos = glm::fvec3(Common::GetChunkPosition(chunk.coords) - position);
                    mesh->Render(this->_renderer, deferredShading, glm::translate(deltaPos), Uint32(glm::lengthSquared(deltaPos)));
                }
            });
    }

}}
