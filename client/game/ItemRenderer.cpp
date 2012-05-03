#include "client/precompiled.hpp"

#include "tools/IRenderer.hpp"
#include "tools/window/Window.hpp"
#include "tools/models/MqmModel.hpp"

#include "client/game/ItemRenderer.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/game/Model.hpp"
#include "client/game/Item.hpp"

#include "common/CubePosition.hpp"

#include "client/Client.hpp"

#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Game {

    ItemRenderer::ItemRenderer(Game& game) :
        _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer()),
        _elapsedTime(0)
    {
        this->_shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("BaseModel.fx");
        this->_shaderTexture = this->_shader->GetParameter("baseTex").release();
        this->_shaderBoneMatrix = this->_shader->GetParameter("boneMatrix").release();
    }

    ItemRenderer::~ItemRenderer()
    {
        Tools::Delete(this->_shaderTexture);
        Tools::Delete(this->_shaderBoneMatrix);
    }

    void ItemRenderer::Render(Item const& item)
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();

        this->_shader->BeginPass();

        float pi = std::atan2(0.0f, -1.0f);

        Common::OrientedPosition const& pos = item.position;

        this->_renderer.SetModelMatrix(
            glm::translate<float>(
                glm::fvec3(pos.position - camera.position)
                )
            *
            glm::yawPitchRoll<float>(
                   pi / 2-pos.theta, -pi / 2, 0.0f
                )
            *
            glm::translate<float>(
                glm::fvec3(-0.5f, -0.5f, -3.5f)
                )
            *
            glm::scale<float>(
                glm::fvec3(0.1f, 0.1f, 0.1f)
                )
            );

        Model const& model = item.GetModel();

        auto textures =  model.GetTextures();
        auto vertexBuffer =  model.GetVertexBuffer();
        auto indexBuffers =  model.GetIndexBuffers();
        auto meshes = model.GetMeshes();
        this->_shaderBoneMatrix->Set(model.GetAnimatedBones());
//        std::cout << "biet0\n";
        vertexBuffer->Bind();
        for (unsigned int i = 0; i < meshes.size(); ++i)
        {
            auto mesh = meshes[i];
//            mesh.texture->Bind();
            textures[i]->Bind();
            this->_shaderTexture->Set(*textures[i]);
//            this->_shaderTexture->Set(*mesh.texture);
//            mesh.vertexBuffer->Bind();
//            mesh.indexBuffer->Bind();

            indexBuffers[i]->Bind();
//            this->_renderer.DrawElements(mesh.indexes.size());
            this->_renderer.DrawElements(mesh.num_triangles*3);

//        this->_renderer.DrawElements(6, Tools::Renderers::DataType::UnsignedInt, (int[6]){0, 1, 2, 3, 4, 5});

            indexBuffers[i]->Unbind();
            textures[i]->Unbind();
//            mesh.indexBuffer->Unbind();
//            mesh.vertexBuffer->Unbind();
//            mesh.texture->Unbind();
//        std::cout << mesh.num_triangles << "\n";
        }

            vertexBuffer->Unbind();
        this->_shader->EndPass();
    }

    void ItemRenderer::Update(Uint32 time)
    {
        this->_elapsedTime += time;
    }

}}
