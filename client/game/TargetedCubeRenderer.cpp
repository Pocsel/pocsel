#include "client/game/TargetedCubeRenderer.hpp"

#include "client/precompiled.hpp"

#include "tools/window/Window.hpp"
#include "tools/IRenderer.hpp"

#include "common/CubePosition.hpp"
#include "common/Position.hpp"

#include "client/game/Game.hpp"
#include "client/game/Player.hpp"

#include "client/Client.hpp"

#include "client/resources/LocalResourceManager.hpp"

namespace Client { namespace Game {

    TargetedCubeRenderer::TargetedCubeRenderer(Game& game) :
        _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer()),
        _elapsedTime(0)
    {
        this->_shader = &this->_game.GetClient().GetLocalResourceManager().GetShader("CubeTarget.fx");
        this->_shaderTexture = &this->_shader->GetParameter("baseTex");
        this->_shaderTime = &this->_shader->GetParameter("time");

        this->_texture = &this->_game.GetClient().GetLocalResourceManager().GetTexture2D("CubeTarget.png");

        this->_InitVertexBuffer();
    }

    TargetedCubeRenderer::~TargetedCubeRenderer()
    {
    }

    void TargetedCubeRenderer::Render(Common::CubePosition const& pos)
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();

        this->_shader->BeginPass();

        this->_renderer.SetModelMatrix(
            glm::translate<float>(
                glm::fvec3(Common::Position(pos) - camera.position)
                )
            );

        this->_vertexBuffer->Bind();
        this->_texture->Bind();
        this->_shaderTexture->Set(*this->_texture);
        this->_shaderTime->Set((float)this->_elapsedTime * 0.000001f);
        this->_renderer.DrawVertexBuffer(0, 6*6);
        this->_texture->Unbind();
        this->_vertexBuffer->Unbind();

        this->_shader->EndPass();
    }

    void TargetedCubeRenderer::Update(Uint64 deltaTime)
    {
        this->_elapsedTime += deltaTime;
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

        this->_vertexBuffer = this->_renderer.CreateVertexBuffer();
        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord0, 2); // texCoord
        this->_vertexBuffer->SetData(6*6*3*sizeof(float) + 6*6*2*sizeof(float), vertices, Tools::Renderers::VertexBufferUsage::Static);
    }

}}
