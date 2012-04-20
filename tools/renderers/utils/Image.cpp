#include "tools/precompiled.hpp"
#include "tools/renderers/utils/Image.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    Image::Image(IRenderer& renderer)
        : _renderer(renderer)
    {
        Image::_InitBuffers(renderer);
    }

    Image::~Image()
    {
    }

    void Image::Render(IShaderParameter& textureParameter, ITexture2D& texture)
    {
        Image::_vertexBuffer->Bind();
        Image::_indexBuffer->Bind();
        texture.Bind();
        textureParameter.Set(texture);

        this->_renderer.DrawElements(4, DataType::UnsignedShort, 0, DrawingMode::TrianglesStrip);

        texture.Unbind();
        Image::_indexBuffer->Unbind();
        Image::_vertexBuffer->Unbind();
    }

    IVertexBuffer* Image::_vertexBuffer = 0;
    IIndexBuffer* Image::_indexBuffer = 0;

    void Image::_InitBuffers(IRenderer& renderer)
    {
        if (Image::_vertexBuffer != 0)
            return;

        static const float vertices[] =
        {
            -1,  1, 0,  0, 1, // left - top
             1,  1, 0,  1, 1, // right - top
             1, -1, 0,  1, 0, // right - bottom
            -1, -1, 0,  0, 0, // left - bottom
        };
        Image::_vertexBuffer = renderer.CreateVertexBuffer().release();
        Image::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3); // position
        Image::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::TexCoord, 2); // texCoord
        Image::_vertexBuffer->SetData(4*(3 + 2)*sizeof(*vertices), vertices, Renderers::VertexBufferUsage::Static);

        static const unsigned short indices[] = { 0, 1, 3, 2 };
        Image::_indexBuffer = renderer.CreateIndexBuffer().release();
        Image::_indexBuffer->SetData(DataType::UnsignedShort, 4*sizeof(*indices), indices);

        renderer.RegisterShutdownCallback([]()
            {
                Tools::Delete(Image::_vertexBuffer);
                Tools::Delete(Image::_indexBuffer);
            });
    }

}}}
