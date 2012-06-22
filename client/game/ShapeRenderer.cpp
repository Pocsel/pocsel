#include "client/game/ShapeRenderer.hpp"

#include "tools/IRenderer.hpp"
#include "tools/window/Window.hpp"

#include "client/game/Game.hpp"
#include "client/game/Player.hpp"

#include "client/Client.hpp"

#include "client/resources/LocalResourceManager.hpp"

#include "common/physics/Node.hpp"

namespace Client { namespace Game {

    ShapeRenderer::ShapeRenderer(Game& game) :
        _game(game),
        _renderer(game.GetClient().GetWindow().GetRenderer()),
        _elapsedTime(0)
    {
        this->_shader = &game.GetClient().GetLocalResourceManager().GetShader("CubeTarget.fx");
        //this->_shaderTexture = &this->_shader->GetParameter("baseTex");
        //this->_shaderTime = &this->_shader->GetParameter("time");

        this->_InitVertexBuffer();
    }

    ShapeRenderer::~ShapeRenderer()
    {
    }

    void ShapeRenderer::Render(Common::Physics::Node const& node)
    {
        auto const& camera = this->_game.GetPlayer().GetCamera();

        this->_shader->BeginPass();

        this->_renderer.SetModelMatrix(
                glm::translate<float>(
                    glm::fvec3(node.position - camera.position)
                    )
                *
                glm::toMat4(node.orientation)
                *
                glm::scale(
                    glm::fvec3(1, 2, 1)
                    )
                );

        this->_vertexBuffer->Bind();
        //this->_texture->Bind();
        //this->_shaderTexture->Set(*this->_texture);
        //this->_shaderTime->Set((float)this->_elapsedTime * 0.000001f);
        this->_renderer.SetRasterizationMode(Tools::Renderers::RasterizationMode::Line);
        this->_renderer.DrawVertexBuffer(0, 6*6);
        this->_renderer.SetRasterizationMode(Tools::Renderers::RasterizationMode::Fill);
        //this->_texture->Unbind();
        this->_vertexBuffer->Unbind();

        this->_shader->EndPass();
    }

    void ShapeRenderer::_InitVertexBuffer()
    {
        const float vertices[] = {
            // front
            -0.5, 0.5, -0.5, // 1.0f, 1.0f,
            0.5, -0.5, -0.5, // 0.0f, 0.0f,
            -0.5, -0.5, -0.5,//  1.0f, 0.0f,

            0.5, 0.5, -0.5,  //0.0f, 1.0f,
            0.5, -0.5, -0.5, // 0.0f, 0.0f,
            -0.5, 0.5, -0.5, // 1.0f, 1.0f,

            // back
            0.5, 0.5, 0.5,  //1.0f, 1.0f,
            -0.5, -0.5, 0.5,//  0.0f, 0.0f,
            0.5, -0.5, 0.5, // 1.0f, 0.0f,

            -0.5, 0.5, 0.5, // 0.0f, 1.0f,
            -0.5, -0.5, 0.5,//  0.0f, 0.0f,
            0.5, 0.5, 0.5,  //1.0f, 1.0f,

            // left
            -0.5, 0.5, 0.5,  //1.0f, 1.0f,
            -0.5, -0.5, -0.5,//  0.0f, 0.0f,
            -0.5, -0.5, 0.5, // 1.0f, 0.0f,

            -0.5, 0.5, -0.5, // 0.0f, 1.0f,
            -0.5, -0.5, -0.5,//  0.0f, 0.0f,
            -0.5, 0.5, 0.5,  //1.0f, 1.0f,

            // right
            0.5, 0.5, -0.5, // 1.0f, 1.0f,
            0.5, -0.5, 0.5, // 0.0f, 0.0f,
            0.5, -0.5, -0.5,//  1.0f, 0.0f,

            0.5, 0.5, 0.5,  //0.0f, 1.0f,
            0.5, -0.5, 0.5, // 0.0f, 0.0f,
            0.5, 0.5, -0.5, // 1.0f, 1.0f,

            // bottom
            -0.5, -0.5, -0.5,//  1.0f, 1.0f,
            0.5, -0.5, 0.5,  //0.0f, 0.0f,
            -0.5, -0.5, 0.5, // 1.0f, 0.0f,

            0.5, -0.5, -0.5, // 0.0f, 1.0f,
            0.5, -0.5, 0.5,  //0.0f, 0.0f,
            -0.5, -0.5, -0.5,//  1.0f, 1.0f,

            // top
            -0.5, 0.5, 0.5, // 1.0f, 1.0f,
            0.5, 0.5, -0.5, // 0.0f, 0.0f,
            -0.5, 0.5, -0.5,//  1.0f, 0.0f,

            0.5, 0.5, 0.5,  //0.0f, 1.0f,
            0.5, 0.5, -0.5, // 0.0f, 0.0f,
            -0.5, 0.5, 0.5, // 1.0f, 1.0f,
        };

        this->_vertexBuffer = this->_renderer.CreateVertexBuffer();
        this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::Position, 3); // position
        //this->_vertexBuffer->PushVertexAttribute(Tools::Renderers::DataType::Float, Tools::Renderers::VertexAttributeUsage::TexCoord, 2); // texCoord
        this->_vertexBuffer->SetData(6*6*3*sizeof(float)/* + 6*6*2*sizeof(float)*/, vertices, Tools::Renderers::VertexBufferUsage::Static);
    }

}}
