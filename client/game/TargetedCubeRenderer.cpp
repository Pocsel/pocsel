#include "client/game/TargetedCubeRenderer.hpp"
#include "client/game/Game.hpp"
#include "client/game/Player.hpp"

#include "tools/IRenderer.hpp"

#include "common/CubePosition.hpp"

#include "client/Client.hpp"

#include "client/resources/LocalResourceManager.hpp"

#include "client/window/Window.hpp"

namespace Client { namespace Game {

    TargetedCubeRenderer::TargetedCubeRenderer(Game& game) :
        _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer()),
        _elapsedTime(0)
    {
        this->_shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("CubeTarget.cgfx");
        this->_shaderTexture = this->_shader->GetParameter("baseTex").release();
        this->_shaderTime = this->_shader->GetParameter("time").release();

        this->_texture = &this->_game.GetClient().GetLocalResourceManager().GetTexture2D("CubeTarget.png");

        this->_InitVertexBuffer();
    }

    TargetedCubeRenderer::~TargetedCubeRenderer()
    {
        Tools::Delete(this->_shaderTexture);
        Tools::Delete(this->_shaderTime);
    }

    void TargetedCubeRenderer::Render(Common::CubePosition const& pos)
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();

        this->_shader->BeginPass();

        this->_renderer.SetModelMatrix(
            Tools::Matrix4<float>::CreateTranslation(
                Common::Position(pos.world, Tools::Vector3f(pos.chunk.x, pos.chunk.y, pos.chunk.z)) - camera.position
                )
            );

        this->_vertexBuffer->Bind();
        this->_texture->Bind();
        this->_shaderTexture->Set(*this->_texture);
        this->_shaderTime->Set((float)this->_elapsedTime * 0.001f);
        this->_renderer.DrawVertexBuffer(0, 6*6);
        this->_texture->Unbind();
        this->_vertexBuffer->Unbind();

        this->_shader->EndPass();
    }

    void TargetedCubeRenderer::Update(Uint32 time)
    {
        this->_elapsedTime += time;
    }

    void TargetedCubeRenderer::_InitVertexBuffer()
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

        this->_vertexBuffer = this->_renderer.CreateVertexBuffer().release();
        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord, 2); // texCoord
        this->_vertexBuffer->SetData(6*6*3*sizeof(float) + 6*6*2*sizeof(float), vertices, Tools::Renderers::VertexBufferUsage::Static);
    }

}}