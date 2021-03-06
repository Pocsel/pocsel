#include "tools/precompiled.hpp"

#include "tools/gfx/utils/Rectangle.hpp"

namespace Tools { namespace Gfx { namespace Utils {

    Rectangle::Rectangle(IRenderer& renderer)
        : _renderer(renderer),
        _point1(-1, -1, 0),
        _point2( 1, -1, 0),
        _point3( 1,  1, 0),
        _point4(-1,  1, 0),
        _point1Color(1, 1, 1, 1),
        _point2Color(1, 1, 1, 1),
        _point3Color(1, 1, 1, 1),
        _point4Color(1, 1, 1, 1)
    {
        this->_vertexBuffer = this->_renderer.CreateVertexBuffer();
        this->_vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Position, 3); // position
        this->_vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Color, 4); // color
        Rectangle::_vertexBuffer->SetData(4*3*sizeof(float) + 4*4*sizeof(float), 0, VertexBufferUsage::Static);
        this->_RefreshVertexBuffer();

        static const unsigned short indices[] = { 2, 1, 3, 0 };
        this->_indexBuffer = this->_renderer.CreateIndexBuffer();
        this->_indexBuffer->SetData(DataType::UnsignedShort, sizeof(indices), indices);
    }

    void Rectangle::Render()
    {
        this->_indexBuffer->Bind();
        this->_vertexBuffer->Bind();
        this->_renderer.DrawElements(4, DataType::UnsignedByte, 0, DrawingMode::TrianglesStrip);
        this->_vertexBuffer->Unbind();
        this->_indexBuffer->Unbind();
    }

    void Rectangle::_RefreshVertexBuffer()
    {
        float vertices[] =
        {
            this->_point1.x, this->_point1.y, this->_point1.z,
            this->_point1Color.r, this->_point1Color.g, this->_point1Color.b, this->_point1Color.a,
            this->_point2.x, this->_point2.y, this->_point2.z,
            this->_point2Color.r, this->_point2Color.g, this->_point2Color.b, this->_point2Color.a,
            this->_point3.x, this->_point3.y, this->_point3.z,
            this->_point3Color.r, this->_point3Color.g, this->_point3Color.b, this->_point3Color.a,
            this->_point4.x, this->_point4.y, this->_point4.z,
            this->_point4Color.r, this->_point4Color.g, this->_point4Color.b, this->_point4Color.a
        };
        this->_vertexBuffer->SetData(sizeof(vertices), vertices, VertexBufferUsage::Static);
    }

}}}
