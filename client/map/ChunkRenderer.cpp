#include "client/precompiled.hpp"

#include "client/Client.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/ChunkRenderer.hpp"
#include "client/map/Map.hpp"
#include "client/resources/LocalResourceManager.hpp"
#include "client/window/Window.hpp"

#include "tools/Frustum.hpp"

namespace Client { namespace Map {

    ChunkRenderer::ChunkRenderer(Game::Game& game)
        : _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer())
    {
        this->_shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("Chunk.cgfx");
        this->_shaderTexture = this->_shader->GetParameter("cubeTexture").release();
        auto const& cubeTypes = this->_game.GetCubeTypeManager().GetCubeTypes();

        for (size_t i = 0; i < cubeTypes.size(); ++i)
        {
            for (Uint8 j = 0; j < 6; ++j)
            {
                Uint32 textureId = cubeTypes[i].textures.ids[j];
                if (this->_textures.find(textureId) == this->_textures.end())
                    this->_textures[textureId] = this->_game.GetResourceManager().CreateTexture(textureId);
            }
        }
    }

    ChunkRenderer::~ChunkRenderer()
    {
        Tools::Delete(this->_shaderTexture);
    }

    bool ChunkRenderer::RefreshGraphics(Chunk& chunk)
    {
        return chunk.GetMesh()->RefreshGraphics(this->_renderer);
    }

    void ChunkRenderer::Update(Uint64 totalTime)
    {
        for (auto texturesIt = this->_textures.begin(), texturesIte = this->_textures.end(); texturesIt != texturesIte; ++texturesIt)
            texturesIt->second->Update(totalTime);
    }

    void ChunkRenderer::Render()
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();
        auto pos = camera.position;
        auto viewProj =
            Tools::Matrix4<double>::CreateLookAt(pos, Tools::Vector3d(pos + Tools::Vector3d(camera.direction)), Tools::Vector3d(0, 1, 0))
            * Tools::Matrix4<double>(camera.projection);
        Tools::Frustum frustum(viewProj);

        std::map<Uint32, std::multimap<double, Chunk*>> transparentChunks;

        do
        {
            this->_shader->BeginPass();
            for (auto texturesIt = this->_textures.begin(), texturesIte = this->_textures.end(); texturesIt != texturesIte; ++texturesIt)
            {
                if (texturesIt->second->HasAlpha())
                {
                    this->_game.GetMap().GetChunkManager().ForeachIn(frustum,
                        [&](Chunk& chunk)
                        {
                            if (chunk.GetMesh() == 0 || chunk.GetMesh()->GetTriangleCount(texturesIt->first) == 0)
                                return;
                            auto const& relativePosition = (Common::GetChunkPosition(chunk.coords) + Tools::Vector3d(Common::ChunkSize / 2.0f)) - camera.position;
                            auto dist = relativePosition.GetMagnitudeSquared();
                            auto value = std::multimap<double, Chunk*>::value_type(-dist, &chunk);
                            transparentChunks[texturesIt->first].insert(value);
                        });
                }
                else
                {
                    texturesIt->second->Bind();
                    this->_shaderTexture->Set(texturesIt->second->GetCurrentTexture());
                    this->_game.GetMap().GetChunkManager().ForeachIn(frustum,
                        [&](Chunk& chunk)
                        {
                            if (chunk.GetMesh() == 0 || chunk.GetMesh()->GetTriangleCount(texturesIt->first) == 0)
                                return;
                            this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(Tools::Vector3f(Common::GetChunkPosition(chunk.coords) - camera.position)));
                            chunk.GetMesh()->Render(texturesIt->first, this->_renderer);
                        });
                    texturesIt->second->Unbind();
                }
            }
        } while (this->_shader->EndPass());

        do
        {
            this->_shader->BeginPass();
            for (auto it = transparentChunks.begin(), ite = transparentChunks.end(); it != ite; ++it)
            {
                auto texture = this->_textures[it->first].get();
                texture->Bind();
                this->_shaderTexture->Set(texture->GetCurrentTexture());
                for (auto itChunk = it->second.begin(), iteChunk = it->second.end(); itChunk != iteChunk; ++itChunk)
                {
                    auto mesh = itChunk->second->GetMesh();
                    if (!mesh)
                        continue;
                    this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(Tools::Vector3f(Common::GetChunkPosition(itChunk->second->coords) - camera.position)));
                    mesh->Render(it->first, this->_renderer);
                }
                texture->Unbind();
            }
        } while (this->_shader->EndPass());
    }
}}
