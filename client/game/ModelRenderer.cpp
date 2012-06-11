#include "client/precompiled.hpp"

#include "tools/Math.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"
#include "tools/IRenderer.hpp"
#include "tools/window/Window.hpp"
#include "tools/models/MqmModel.hpp"

#include "client/game/ModelRenderer.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"
#include "client/game/engine/Model.hpp"

#include "common/CubePosition.hpp"

#include "client/Client.hpp"

#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Game {

    ModelRenderer::ModelRenderer(Game& game) :
        _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer())
    {
        this->_shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("BaseModel.fx");
        this->_shaderTexture = this->_shader->GetParameter("diffuseTexture").release();
        this->_shaderBoneMatrix = this->_shader->GetParameter("boneMatrix").release();
    }

    ModelRenderer::~ModelRenderer()
    {
        Tools::Delete(this->_shaderTexture);
        Tools::Delete(this->_shaderBoneMatrix);
    }

    void ModelRenderer::Render(Engine::Model const& model, Common::Position const& pos)
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();

        this->_shader->BeginPass();

        //Common::OrientedPosition const& pos = item.position;

        this->_renderer.SetModelMatrix(
            glm::translate<float>(
                glm::fvec3(pos - camera.position)
                )
            *
            glm::yawPitchRoll<float>(
                   /*-pos.theta*/0, -Tools::Math::PiFloat / 2, 0.0f
                )
            //*
            //glm::translate<float>(
            //    glm::fvec3(-0.5f, -0.5f, -3.5f)
            //    )
            //*
            //glm::scale<float>(
            //    glm::fvec3(0.1f, 0.1f, 0.1f)
            //    )
            );

        //Engine::Model const& model = item.GetModel();

        auto textures =  model.GetTextures();
        auto vertexBuffer =  model.GetVertexBuffer();
        auto indexBuffers =  model.GetIndexBuffers();
        auto meshes = model.GetMeshes();
        this->_shaderBoneMatrix->Set(model.GetAnimatedBones());
        vertexBuffer->Bind();
        for (unsigned int i = 0; i < meshes.size(); ++i)
        {
            auto& mesh = meshes[i];
            textures[i]->Bind();
            this->_shaderTexture->Set(textures[i]->GetCurrentTexture());
            indexBuffers[i]->Bind();
            this->_renderer.DrawElements(mesh.num_triangles * 3);
            indexBuffers[i]->Unbind();
            textures[i]->Unbind();
        }
        vertexBuffer->Unbind();

        this->_shader->EndPass();
    }

}}
