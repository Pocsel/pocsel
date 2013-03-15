#include "tools/precompiled.hpp"
#include "tools/gfx/utils/Image.hpp"

namespace Tools { namespace Gfx { namespace Utils {

    Image::Image(IRenderer& renderer)
        : _renderer(renderer)
    {
        this->SetTextureCoords(glm::vec2(0.0f), glm::vec2(1.0f));
    }

    Image::~Image()
    {
    }

    void Image::Render(IShaderParameter& textureParameter, ITexture2D& texture)
    {
        this->_vertexBuffer->Bind();
        this->_indexBuffer->Bind();
        texture.Bind();
        textureParameter.Set(texture);

        this->_renderer.DrawElements(6, DataType::UnsignedShort);

        texture.Unbind();
        this->_indexBuffer->Unbind();
        this->_vertexBuffer->Unbind();
    }

    void Image::Render()
    {
        this->_vertexBuffer->Bind();
        this->_indexBuffer->Bind();

        this->_renderer.DrawElements(6, DataType::UnsignedShort);

        this->_indexBuffer->Unbind();
        this->_vertexBuffer->Unbind();
    }

    void Image::SetTextureCoords(glm::vec2 tl, glm::vec2 br)
    {
        float vertices[] =
        {
            -0.5f,  0.5f, 0,  tl.x, br.y, // left - top
             0.5f,  0.5f, 0,  br.x, br.y, // right - top
             0.5f, -0.5f, 0,  br.x, tl.y, // right - bottom
            -0.5f, -0.5f, 0,  tl.x, tl.y, // left - bottom
        };
        this->_vertexBuffer = this->_renderer.CreateVertexBuffer();
        this->_vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Position, 3); // position
        this->_vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::TexCoord0, 2); // texCoord
        this->_vertexBuffer->SetData(sizeof(vertices), vertices, VertexBufferUsage::Static);

        if (!this->_indexBuffer)
        {
            static const unsigned short indices[] = { 0, 1, 3, 1, 2, 3 };
            this->_indexBuffer = this->_renderer.CreateIndexBuffer();
            this->_indexBuffer->SetData(DataType::UnsignedShort, sizeof(indices), indices);
        }
    }

}}}
