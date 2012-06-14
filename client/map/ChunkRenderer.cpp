#include "client/precompiled.hpp"

#include "tools/window/Window.hpp"
#include "tools/Frustum.hpp"
#include "tools/lua/Interpreter.hpp"

#include "client/Client.hpp"
#include "client/game/CubeType.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/ChunkRenderer.hpp"
#include "client/map/Map.hpp"
#include "client/resources/Effect.hpp"
#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Map {

    namespace {
        bool compareDistances(std::pair<double, Chunk*> const& p1, std::pair<double, Chunk*> const& p2)
        {
            return p1.first > p2.first;
        }
    }

    ChunkRenderer::ChunkRenderer(Game::Game& game)
        : _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer())
    {
        auto const& cubeTypes = this->_game.GetCubeTypeManager().GetCubeTypes();

        for (size_t i = 0; i < cubeTypes.size(); ++i)
        {
            for (Uint8 j = 0; j < 6; ++j)
            {
                Uint32 textureId = cubeTypes[i].textures.ids[j];
                if (this->_textures.find(textureId) == this->_textures.end())
                    this->_textures[textureId] = this->_game.GetResourceManager().GetTexture(textureId);
                if (this->_cubeTypes.find(cubeTypes[i].effects.effects[j]) == this->_cubeTypes.end())
                {
                    this->_cubeTypes[cubeTypes[i].effects.effects[j]] =
                        std::make_pair(
                            &cubeTypes[i].effects.effects[j]->GetParameter("cubeTexture"),
                            std::map<Uint32, Tools::Renderers::Utils::Texture::ITexture*>());
                }
                this->_cubeTypes[cubeTypes[i].effects.effects[j]].second[textureId] = this->_textures[textureId].get();
            }
        }
    }

    ChunkRenderer::~ChunkRenderer()
    {
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

    void ChunkRenderer::Render(Common::Position const& position, glm::dmat4 viewProj)
    {
        //auto const& camera = this->_game.GetPlayer().GetCamera();
        //auto pos = camera.position;
        //auto viewProj =
        //    glm::detail::tmat4x4<double>(camera.projection)
        //    * glm::lookAt<double>(pos, glm::dvec3(pos + glm::dvec3(camera.direction)), glm::dvec3(0, 1, 0));

        static std::vector<std::pair<double, Chunk*>> visibleChunks;
        visibleChunks.clear();
        this->_game.GetMap().GetChunkManager().ForeachIn(Tools::Frustum(viewProj),
            [&](Chunk& chunk)
            {
                if (chunk.GetMesh() == 0 || chunk.GetMesh()->GetTriangleCount() == 0)
                    return;
                auto const& relativePosition = (Common::GetChunkPosition(chunk.coords) + glm::dvec3(Common::ChunkSize / 2.0f)) - position;
                auto dist = glm::lengthSquared(relativePosition);
                visibleChunks.push_back(std::make_pair(dist, &chunk));
            });

        this->_transparentChunks.clear();
        if (visibleChunks.size() == 0)
            return;
        std::sort(visibleChunks.begin(), visibleChunks.end(), &compareDistances);

        for (auto effectIt = this->_cubeTypes.begin(), effectIte = this->_cubeTypes.end(); effectIt != effectIte; ++effectIt)
        {
            do
            {
                effectIt->first->BeginPass();
                for (auto texturesIt = effectIt->second.second.begin(), texturesIte = effectIt->second.second.end(); texturesIt != texturesIte; ++texturesIt)
                {
                    if (texturesIt->second->HasAlpha())
                    {
                        for (auto chunkIt = visibleChunks.begin(), chunkIte = visibleChunks.end(); chunkIt != chunkIte; ++chunkIt)
                        {
                            auto& chunk = chunkIt->second;
                            if (chunk->GetMesh() == 0 || chunk->GetMesh()->GetTriangleCount(texturesIt->first) == 0)
                                continue;
                            this->_transparentChunks[texturesIt->first].insert(std::multimap<double, Chunk*>::value_type(-chunkIt->first/*dist*/, chunk));
                        }
                    }
                    else
                    {
                        texturesIt->second->Bind();
                        effectIt->second.first->Set(texturesIt->second->GetCurrentTexture());
                        for (auto chunkIt = visibleChunks.begin(), chunkIte = visibleChunks.end(); chunkIt != chunkIte; ++chunkIt)
                        {
                            auto& chunk = chunkIt->second;
                            if (chunk->GetMesh() == 0 || chunk->GetMesh()->GetTriangleCount(texturesIt->first) == 0)
                                continue;
                            effectIt->first->Update(this->_game.GetInterpreter().MakeNil()); // TODO: biome data
                            this->_renderer.SetModelMatrix(glm::translate<float>(glm::fvec3(Common::GetChunkPosition(chunk->coords) - position)));
                            chunk->GetMesh()->Render(texturesIt->first, this->_renderer);
                        }
                        texturesIt->second->Unbind();
                    }
                }
            } while (effectIt->first->EndPass());
        }
    }

    void ChunkRenderer::RenderAlpha(Common::Position const& position)
    {
        if (this->_transparentChunks.size() == 0)
            return;

        //auto const& camera = this->_game.GetPlayer().GetCamera();
        for (auto effectIt = this->_cubeTypes.begin(), effectIte = this->_cubeTypes.end(); effectIt != effectIte; ++effectIt)
        {
            do
            {
                effectIt->first->BeginPass();
                for (auto it = this->_transparentChunks.begin(), ite = this->_transparentChunks.end(); it != ite; ++it)
                {
                    auto texture = effectIt->second.second[it->first];
                    texture->Bind();
                    effectIt->second.first->Set(texture->GetCurrentTexture());
                    for (auto itChunk = it->second.begin(), iteChunk = it->second.end(); itChunk != iteChunk; ++itChunk)
                    {
                        auto mesh = itChunk->second->GetMesh();
                        if (!mesh)
                            continue;
                        this->_renderer.SetModelMatrix(glm::translate<float>(glm::fvec3(Common::GetChunkPosition(itChunk->second->coords) - position)));
                        mesh->Render(it->first, this->_renderer);
                    }
                    texture->Unbind();
                }
            } while (effectIt->first->EndPass());
        }
    }
}}
