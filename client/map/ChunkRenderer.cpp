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
                    this->_textures[textureId] = &this->_game.GetResourceManager().GetTexture2D(textureId);
            }
        }
    }

    ChunkRenderer::~ChunkRenderer()
    {
        Tools::Delete(this->_shaderTexture);
        for (auto it = this->_textures.begin(), ite = this->_textures.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    void ChunkRenderer::RefreshDisplay(Chunk& chunk)
    {
        ChunkMesh* mesh = chunk.GetMesh();
        if (mesh == 0)
        {
            mesh = new ChunkMesh(chunk);
            chunk.SetMesh(std::unique_ptr<ChunkMesh>(mesh));
        }
        mesh->Refresh(this->_game, *this);
    }

    void ChunkRenderer::Render()
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();
        auto pos = camera.position.GetVector<double>();
        auto viewProj = 
            Tools::Matrix4<double>::CreateLookAt(pos, Tools::Vector3d(pos + Tools::Vector3d(camera.direction)), Tools::Vector3d(0, 1, 0))
            * Tools::Matrix4<double>(camera.projection);
        Tools::Frustum frustum(viewProj);

        std::multimap<double, Chunk*> transparentChunks;

        do
        {
            this->_shader->BeginPass();
            for (auto texturesIt = this->_textures.begin(), texturesIte = this->_textures.end(); texturesIt != texturesIte; ++texturesIt)
            {
                texturesIt->second->Bind();
                this->_shaderTexture->Set(*texturesIt->second);
                this->_game.GetMap().GetChunkManager().ForeachIn(frustum,
                    [&](Chunk& chunk)
                    {
                        if (chunk.GetMesh() == 0 || chunk.GetMesh()->GetTriangleCount() == 0)
                            return;
                        if (chunk.GetMesh()->HasTransparentCube())
                        {
                            auto const& relativePosition = Common::Position(chunk.coords, Tools::Vector3f(Common::ChunkSize / 2.0f)) - camera.position;
                            auto dist = relativePosition.GetMagnitudeSquared();
                            auto value = std::multimap<double, Chunk*>::value_type(-dist, &chunk);
                            transparentChunks.insert(value);
                        }
                        if (!texturesIt->second->HasAlpha())
                        {
                            this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(Common::Position(chunk.coords, Tools::Vector3f(0)) - camera.position));
                            chunk.GetMesh()->Render(texturesIt->first, this->_renderer);
                        }
                    });
                texturesIt->second->Unbind();
            }
        } while (this->_shader->EndPass());

        do
        {
            this->_shader->BeginPass();
            for (auto texturesIt = this->_textures.begin(), texturesIte = this->_textures.end(); texturesIt != texturesIte; ++texturesIt)
            {
                if (!texturesIt->second->HasAlpha())
                    continue;
                texturesIt->second->Bind();
                this->_shaderTexture->Set(*texturesIt->second);
                for (auto it = transparentChunks.begin(), ite = transparentChunks.end(); it != ite; ++it)
                {
                    auto mesh = (*it).second->GetMesh();
                    if (!mesh)
                        continue;
                    this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(Common::Position((*it).second->coords, Tools::Vector3f(0)) - camera.position));
                    mesh->Render(texturesIt->first, this->_renderer);
                }
                texturesIt->second->Unbind();
            }
        } while (this->_shader->EndPass());
    }
}}
