#include "client/game/ItemRenderer.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"

#include "tools/IRenderer.hpp"

#include "common/CubePosition.hpp"

#include "client/Client.hpp"

#include "client/resources/LocalResourceManager.hpp"

#include "client/window/Window.hpp"

namespace Client { namespace Game {

    ItemRenderer::ItemRenderer(Game& game) :
        _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer()),
        _elapsedTime(0)
    {
        this->_shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("BaseModel.cgfx");
        this->_shaderTexture = this->_shader->GetParameter("baseTex").release();
        this->_shaderBoneMatrix = this->_shader->GetParameter("boneMatrix").release();
        //this->_shaderTime = this->_shader->GetParameter("time").release();
        this->_shaderTime = 0;

        //this->_texture = &this->_game.GetClient().GetLocalResourceManager().GetTexture2D("test.png");
        this->_texture = 0;

        this->_md5Model = this->_game.GetClient().GetLocalResourceManager().GetMd5Model("boblampclean");

        //this->_md5Model.LoadModel("");
        //this->_md5Model.LoadAnim("");
    }

    ItemRenderer::~ItemRenderer()
    {
        Tools::Delete(this->_shaderTexture);
        Tools::Delete(this->_shaderTime);
        Tools::Delete(this->_md5Model);
    }

    void ItemRenderer::Render(Common::OrientedPosition const& pos)
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();

        this->_shader->BeginPass();

        float pi = std::atan2(0.0f, -1.0f);

        this->_renderer.SetModelMatrix(
            glm::translate<float>(
                glm::fvec3(pos.position - camera.position)
                )
            *
            glm::yawPitchRoll<float>(
                   pi / 2-pos.theta, -pi +pos.phi, 0.0f//-pos.phi
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

        //this->_md5Model->Render(this->_renderer);

        auto meshes = this->_md5Model->GetMeshes();
        this->_shaderBoneMatrix->Set(this->_md5Model->GetAnimatedBones());
        for (auto it = meshes.begin(), ite = meshes.end(); it != ite; ++it)
        {
            auto mesh = *it;
            mesh.texture->Bind();
            this->_shaderTexture->Set(*mesh.texture);
//            this->_shaderTime->Set((float)this->_elapsedTime * 0.001f);

//        glDisable(GL_CULL_FACE);
            mesh.vertexBuffer->Bind();
            mesh.indexBuffer->Bind();

            this->_renderer.DrawElements(mesh.indexes.size());

//        glEnable(GL_CULL_FACE);

            mesh.indexBuffer->Unbind();
            mesh.vertexBuffer->Unbind();
            mesh.texture->Unbind();
        }

//        this->_vertexBuffer->Bind();
//        this->_texture->Bind();
//        this->_shaderTexture->Set(*this->_texture);
//        this->_shaderTime->Set((float)this->_elapsedTime * 0.001f);
//        this->_renderer.DrawVertexBuffer(0, 6*6);
//        this->_texture->Unbind();
//        this->_vertexBuffer->Unbind();

        this->_shader->EndPass();
    }

    void ItemRenderer::Update(Uint32 time)
    {
        this->_md5Model->Update(time);
        //this->_elapsedTime += time;
    }

}}

