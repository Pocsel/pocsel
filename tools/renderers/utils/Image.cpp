#include "tools/precompiled.hpp"
#include "resources/resources.hpp"
#include "tools/renderers/utils/Image.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    Image::Image(IRenderer& renderer)
        : _renderer(renderer)
    {
        Image::_InitVertexBuffer(renderer);
    }

    Image::~Image()
    {
    }

    void Image::Render(IShaderParameter& textureParameter, ITexture2D& texture)
    {
        static const unsigned char indices[] = { 2, 1, 3, 0 };

        Image::_vertexBuffer->Bind();
        texture.Bind();
        textureParameter.Set(texture);

        this->_renderer.UpdateCurrentParameters();
        this->_renderer.DrawElements(sizeof(indices), Renderers::DataType::UnsignedByte, indices, Renderers::DrawingMode::TrianglesStrip);

        texture.Unbind();
        Image::_vertexBuffer->Unbind();
    }

    IVertexBuffer* Image::_vertexBuffer = 0;

    void Image::_InitVertexBuffer(IRenderer& renderer)
    {
        if (Image::_vertexBuffer != 0)
            return;

        const float vertices[] =
        {
            -1,  1, 0,  0, 0,
             1,  1, 0,  1, 0,
             1, -1, 0,  1, 1,
            -1, -1, 0,  0, 1,
        };
        Image::_vertexBuffer = renderer.CreateVertexBuffer().release();
        Image::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3); // position
        Image::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::TexCoord, 2); // texCoord
        Image::_vertexBuffer->SetData(4*3*sizeof(float) + 4*2*sizeof(float), vertices, Renderers::VertexBufferUsage::Static);
    }

}}}
