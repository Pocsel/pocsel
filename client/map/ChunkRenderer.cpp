#include "client/precompiled.hpp"

#include "client/Client.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/map/Chunk.hpp"
#include "client/map/ChunkRenderer.hpp"
#include "client/map/Map.hpp"
#include "client/resources/LocalResourceManager.hpp"
#include "client/resources/ResourceManager.hpp"
#include "client/window/Window.hpp"

#include "tools/Frustum.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"

namespace Client { namespace Map {

    ChunkRenderer::Effect::Effect(Resources::ResourceManager& resourceManager, Uint32 id)
        : shader(0),
        shaderTexture(0),
        shaderNormalMap(0),
        shaderTime(0)
    {
        try
        {
            Tools::Lua::Interpreter i;
            i.DoString(resourceManager.GetScript(id));
            this->shader = &resourceManager.GetShader(resourceManager.GetPluginId(id), i.Globals()["shader"].ToString());
            for (auto it = i.Globals().Begin(), ite = i.Globals().End(); it != ite; ++it)
            {
                if (it.GetKey().ToString() == "textureParameter")
                    this->shaderTexture = this->shader->GetParameter(it.GetValue().ToString()).release();
                else if (it.GetKey().ToString() == "normalMapParameter")
                    this->shaderNormalMap = this->shader->GetParameter(it.GetValue().ToString()).release();
                else if (it.GetKey().ToString() == "timeParameter")
                    this->shaderTime = this->shader->GetParameter(it.GetValue().ToString()).release();
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "Fail to load script id " << id << " from cache, check your plugin or tccache file.\n";
            throw;
        }
    }

    ChunkRenderer::Effect::Effect(Tools::Renderers::IShaderProgram* shader, std::string const& texture, std::string const& normalMap, std::string const& time)
        : shader(0),
        shaderTexture(0),
        shaderNormalMap(0),
        shaderTime(0)
    {
        this->shader = shader;
        this->shaderTexture = shader->GetParameter(texture).release();
        this->shaderNormalMap = shader->GetParameter(normalMap).release();
        this->shaderTime = shader->GetParameter(time).release();
    }

    ChunkRenderer::Effect::Effect(ChunkRenderer::Effect&& effect)
        : shader(0),
        shaderTexture(0),
        shaderNormalMap(0),
        shaderTime(0)
    {
        std::swap(this->shader, effect.shader);
        std::swap(this->shaderTexture, effect.shaderTexture);
        std::swap(this->shaderNormalMap, effect.shaderNormalMap);
        std::swap(this->shaderTime, effect.shaderTime);
    }

    ChunkRenderer::Effect::~Effect()
    {
        Tools::Delete(this->shaderTexture);
        Tools::Delete(this->shaderNormalMap);
        Tools::Delete(this->shaderTime);
    }

    ChunkRenderer::ChunkRenderer(Game::Game& game)
        : _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer())
    {
        auto shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("Chunk.cgfx");

        this->_shaderTexture = this->_shader->GetParameter("cubeTexture").release();
        this->_shaderTime = this->_shader->GetParameter("time").release();
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
        Tools::Delete(this->_shaderTime);
    }

    bool ChunkRenderer::RefreshGraphics(Chunk& chunk)
    {
        return chunk.GetMesh()->RefreshGraphics(this->_renderer);
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

        this->_shaderTime->Set(this->_tmpTimer.GetPreciseElapsedTime() * 0.000001f);
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
