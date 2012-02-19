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
#include "tools/renderers/utils/TextureAtlas.hpp"

namespace Client { namespace Map {

    ChunkRenderer::ChunkRenderer(Game::Game& game)
        : _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer()),
        _atlas(0)
    {
        this->_shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("Chunk.cgfx");
        this->_shaderTexture = this->_shader->GetParameter("cubeTexture").release();
        auto const& cubeTypes = this->_game.GetCubeTypeManager().GetCubeTypes();

        std::list<Uint32> textures;
        for (size_t i = 0; i < cubeTypes.size(); ++i)
        {
            textures.push_back(cubeTypes[i].textures.top);
            textures.push_back(cubeTypes[i].textures.bottom);
            textures.push_back(cubeTypes[i].textures.front);
            textures.push_back(cubeTypes[i].textures.back);
            textures.push_back(cubeTypes[i].textures.left);
            textures.push_back(cubeTypes[i].textures.right);
        }
        this->_atlas = game.GetResourceManager().CreateTextureAtlas(textures).release();

        this->_cubeInfo.resize(cubeTypes.size());
        for (size_t i = 0; i < cubeTypes.size(); ++i)
        {
            this->_cubeInfo[i].id = cubeTypes[i].id;
            //this->_cubeInfo[i].texture = &this->_game.GetResourceManager().GetTexture2D(cubeTypes[i].textures.top);
            this->_cubeInfo[i].top = this->_atlas->GetCoords(cubeTypes[i].textures.top);
            this->_cubeInfo[i].bottom = this->_atlas->GetCoords(cubeTypes[i].textures.bottom);
            this->_cubeInfo[i].front = this->_atlas->GetCoords(cubeTypes[i].textures.front);
            this->_cubeInfo[i].back = this->_atlas->GetCoords(cubeTypes[i].textures.back);
            this->_cubeInfo[i].left = this->_atlas->GetCoords(cubeTypes[i].textures.left);
            this->_cubeInfo[i].right = this->_atlas->GetCoords(cubeTypes[i].textures.right);
            this->_cubeInfo[i].isTransparent = 
                this->_atlas->HasAlpha(cubeTypes[i].textures.top) ||
                this->_atlas->HasAlpha(cubeTypes[i].textures.bottom) ||
                this->_atlas->HasAlpha(cubeTypes[i].textures.front) ||
                this->_atlas->HasAlpha(cubeTypes[i].textures.back) ||
                this->_atlas->HasAlpha(cubeTypes[i].textures.left) ||
                this->_atlas->HasAlpha(cubeTypes[i].textures.right);
        }
    }

    ChunkRenderer::~ChunkRenderer()
    {
        Tools::Delete(this->_shaderTexture);
        Tools::Delete(this->_atlas);
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

        std::deque<Chunk*> chunkToRender;
        std::multimap<double, Chunk*> transparentChunks;
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
                else
                    chunkToRender.push_back(&chunk);
            });

        this->_atlas->GetTexture().Bind();
        this->_shaderTexture->Set(this->_atlas->GetTexture());
        do
        {
            this->_shader->BeginPass();
            for (auto it = chunkToRender.begin(), ite = chunkToRender.end(); it != ite; ++it)
            {
                auto mesh = (*it)->GetMesh();
                if (!mesh)
                    continue;
                this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(Common::Position((*it)->coords, Tools::Vector3f(0)) - camera.position));
                mesh->Render(0, this->_renderer);
            }
            for (auto it = transparentChunks.begin(), ite = transparentChunks.end(); it != ite; ++it)
            {
                auto mesh = (*it).second->GetMesh();
                if (!mesh)
                    continue;
                this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(Common::Position((*it).second->coords, Tools::Vector3f(0)) - camera.position));
                mesh->Render(0, this->_renderer);
            }
        } while (this->_shader->EndPass());
        this->_atlas->GetTexture().Unbind();

        //do
        //{
        //    this->_shader->BeginPass();
        //    this->_baseVbo->Bind();
        //    for (size_t i = 0; i < this->_cubeInfo.size(); ++i)
        //    {
        //        if (!this->_cubeInfo[i].isTransparent)
        //            continue;
        //        this->_cubeInfo[i].texture->Bind();
        //        this->_shaderTexture->Set(*this->_cubeInfo[i].texture);

        //        for (auto it = transparentChunks.begin(), ite = transparentChunks.end(); it != ite; ++it)
        //        {
        //            auto mesh = (*it).second->GetMesh();
        //            if (!mesh || !mesh->HasCubeType(this->_cubeInfo[i].id))
        //                continue;

        //            this->_renderer.SetModelMatrix(Tools::Matrix4<float>::CreateTranslation(Common::Position((*it).second->coords, Tools::Vector3f(0)) - camera.position));
        //            mesh->Render(this->_cubeInfo[i].id, this->_renderer);
        //        }

        //        this->_cubeInfo[i].texture->Unbind();
        //    }
        //    this->_baseVbo->Unbind();
        //} while (this->_shader->EndPass());
    }
}}
