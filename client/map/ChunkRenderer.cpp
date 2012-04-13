#include "client/precompiled.hpp"

#include "client/Client.hpp"
#include "client/game/CubeType.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/ChunkRenderer.hpp"
#include "client/map/Map.hpp"
#include "client/resources/Effect.hpp"
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
                this->_cubeTypes[cubeTypes[i].effects.effects[j]][textureId] = this->_textures[textureId].get();
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

        std::list<Chunk*> visibleChunks;
        this->_game.GetMap().GetChunkManager().ForeachIn(Tools::Frustum(viewProj),
            [&](Chunk& chunk)
            {
                if (chunk.GetMesh() == 0 || chunk.GetMesh()->GetTriangleCount() == 0)
                    return;
                visibleChunks.push_back(&chunk);
            });
        if (visibleChunks.size() == 0)
            return;

        this->_transparentChunks.clear();
        for (auto effectIt = this->_cubeTypes.begin(), effectIte = this->_cubeTypes.end(); effectIt != effectIte; ++effectIt)
        {
            do
            {
                effectIt->first->BeginPass();
                for (auto texturesIt = effectIt->second.begin(), texturesIte = effectIt->second.end(); texturesIt != texturesIte; ++texturesIt)
                {
                    if (texturesIt->second->HasAlpha())
                    {
                        for (auto chunkIt = visibleChunks.begin(), chunkIte = visibleChunks.end(); chunkIt != chunkIte; ++chunkIt)
                        {
                            if ((*chunkIt)->GetMesh() == 0 || (*chunkIt)->GetMesh()->GetTriangleCount(texturesIt->first) == 0)
                                continue;
                            auto chunkPos = Common::Position((*chunkIt)->coords) * Common::ChunkSize + Common::Position(Common::ChunkSize / 2.0);
                            auto const& relativePosition = chunkPos - camera.position;
                            auto dist = relativePosition.GetMagnitudeSquared();
                            this->_transparentChunks[texturesIt->first].insert(std::multimap<double, Chunk*>::value_type(-dist, *chunkIt));
                        }
                    }
                    else
                    {
                        texturesIt->second->Bind();
                        this->_shaderTexture->Set(texturesIt->second->GetCurrentTexture());
                        for (auto chunkIt = visibleChunks.begin(), chunkIte = visibleChunks.end(); chunkIt != chunkIte; ++chunkIt)
                        {
                            if ((*chunkIt)->GetMesh() == 0 || (*chunkIt)->GetMesh()->GetTriangleCount(texturesIt->first) == 0)
                                continue;
                            effectIt->first->Update(this->_game.GetInterpreter().MakeNil()); // TODO: biome data
                            auto chunkPos = Common::Position((*chunkIt)->coords) * Common::ChunkSize;
                            this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(Tools::Vector3f(chunkPos - camera.position)));
                            (*chunkIt)->GetMesh()->Render(texturesIt->first, this->_renderer);
                        }
                        texturesIt->second->Unbind();
                    }
                }
            } while (effectIt->first->EndPass());
        }
    }

    void ChunkRenderer::RenderAlpha()
    {
        if (this->_transparentChunks.size() == 0)
            return;

        auto const& camera = this->_game.GetPlayer().GetCamera();
        for (auto effectIt = this->_cubeTypes.begin(), effectIte = this->_cubeTypes.end(); effectIt != effectIte; ++effectIt)
        {
            do
            {
                effectIt->first->BeginPass();
                for (auto it = this->_transparentChunks.begin(), ite = this->_transparentChunks.end(); it != ite; ++it)
                {
                    auto texture = effectIt->second[it->first];
                    texture->Bind();
                    this->_shaderTexture->Set(texture->GetCurrentTexture());
                    for (auto itChunk = it->second.begin(), iteChunk = it->second.end(); itChunk != iteChunk; ++itChunk)
                    {
                        auto mesh = itChunk->second->GetMesh();
                        if (!mesh)
                            continue;
                        Tools::Vector3f relativePos(Common::Position(itChunk->second->coords) * Common::ChunkSize - camera.position);
                        this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(relativePos));
                        mesh->Render(it->first, this->_renderer);
                    }
                    texture->Unbind();
                }
            } while (effectIt->first->EndPass());
        }
    }
}}
