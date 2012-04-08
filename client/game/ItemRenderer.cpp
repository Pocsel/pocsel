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
        this->_shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("CubeTarget.cgfx");
        this->_shaderTexture = this->_shader->GetParameter("baseTex").release();
        this->_shaderTime = this->_shader->GetParameter("time").release();

        this->_texture = &this->_game.GetClient().GetLocalResourceManager().GetTexture2D("test.png");

        this->_md5Model = this->_game.GetClient().GetLocalResourceManager().GetMd5Model("boblampclean");

        //this->_md5Model.LoadModel("");
        //this->_md5Model.LoadAnim("");

        //this->_InitVertexBuffer();
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

        this->_renderer.SetModelMatrix(
            Tools::Matrix4<float>::CreateTranslation(
                Tools::Vector3f(-0.5f, -0.5f, -0.5f)
                )
            *
            Tools::Matrix4<float>::CreateYawPitchRollRotation(
                    -pos.theta, 0.0f, -pos.phi
                )
            *
            Tools::Matrix4<float>::CreateTranslation(
                Tools::Vector3f(pos.position - camera.position)
                )
            );

        //this->_md5Model->Render(this->_renderer);

        auto meshes = this->_md5Model->GetMeshes();
        for (auto it = meshes.begin(), ite = meshes.end(); it != ite; ++it)
        {
            auto mesh = *it;
            mesh.texture->Bind();
            this->_shaderTexture->Set(*mesh.texture);
//            this->_shaderTime->Set((float)this->_elapsedTime * 0.001f);

        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glEnableClientState( GL_NORMAL_ARRAY );

        glVertexPointer( 3, GL_FLOAT, 0, &(mesh.positionBuffer[0]) );
        glNormalPointer( GL_FLOAT, 0, &(mesh.normalBuffer[0]) );
        glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.tex2DBuffer[0]) );

        this->_renderer.DrawElements( mesh.indexBuffer.size(), Tools::Renderers::DataType::UnsignedInt, &(mesh.indexBuffer[0]), Tools::Renderers::DrawingMode::Triangles);

        glDisableClientState( GL_NORMAL_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        glDisableClientState( GL_VERTEX_ARRAY );

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
        this->_elapsedTime += time;
    }

    void ItemRenderer::_InitVertexBuffer()
    {
        const float vertices[] = {
            // front
            -0.001f, 1.001f, -0.001f,  1.0f, 1.0f,
            1.001f, -0.001f, -0.001f,  0.0f, 0.0f,
            -0.001f, -0.001f, -0.001f,  1.0f, 0.0f,

            1.001f, 1.001f, -0.001f,  0.0f, 1.0f,
            1.001f, -0.001f, -0.001f,  0.0f, 0.0f,
            -0.001f, 1.001f, -0.001f,  1.0f, 1.0f,

            // back
            1.001f, 1.001f, 1.001f,  1.0f, 1.0f,
            -0.001f, -0.001f, 1.001f,  0.0f, 0.0f,
            1.001f, -0.001f, 1.001f,  1.0f, 0.0f,

            -0.001f, 1.001f, 1.001f,  0.0f, 1.0f,
            -0.001f, -0.001f, 1.001f,  0.0f, 0.0f,
            1.001f, 1.001f, 1.001f,  1.0f, 1.0f,

            // left
            -0.001f, 1.001f, 1.001f,  1.0f, 1.0f,
            -0.001f, -0.001f, -0.001f,  0.0f, 0.0f,
            -0.001f, -0.001f, 1.001f,  1.0f, 0.0f,

            -0.001f, 1.001f, -0.001f,  0.0f, 1.0f,
            -0.001f, -0.001f, -0.001f,  0.0f, 0.0f,
            -0.001f, 1.001f, 1.001f,  1.0f, 1.0f,

            // right
            1.001f, 1.001f, -0.001f,  1.0f, 1.0f,
            1.001f, -0.001f, 1.001f,  0.0f, 0.0f,
            1.001f, -0.001f, -0.001f,  1.0f, 0.0f,

            1.001f, 1.001f, 1.001f,  0.0f, 1.0f,
            1.001f, -0.001f, 1.001f,  0.0f, 0.0f,
            1.001f, 1.001f, -0.001f,  1.0f, 1.0f,

            // bottom
            -0.001f, -0.001f, -0.001f,  1.0f, 1.0f,
            1.001f, -0.001f, 1.001f,  0.0f, 0.0f,
            -0.001f, -0.001f, 1.001f,  1.0f, 0.0f,

            1.001f, -0.001f, -0.001f,  0.0f, 1.0f,
            1.001f, -0.001f, 1.001f,  0.0f, 0.0f,
            -0.001f, -0.001f, -0.001f,  1.0f, 1.0f,

            // top
            -0.001f, 1.001f, 1.001f,  1.0f, 1.0f,
            1.001f, 1.001f, -0.001f,  0.0f, 0.0f,
            -0.001f, 1.001f, -0.001f,  1.0f, 0.0f,

            1.001f, 1.001f, 1.001f,  0.0f, 1.0f,
            1.001f, 1.001f, -0.001f,  0.0f, 0.0f,
            -0.001f, 1.001f, 1.001f,  1.0f, 1.0f,
        };

//        this->_vertexBuffer = this->_renderer.CreateVertexBuffer();
//        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
//        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord, 2); // texCoord
//        this->_vertexBuffer->SetData(6*6*3*sizeof(float) + 6*6*2*sizeof(float), vertices, Tools::Renderers::VertexBufferUsage::Static);
    }

}}

