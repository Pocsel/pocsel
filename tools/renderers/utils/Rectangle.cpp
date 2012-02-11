#include "tools/precompiled.hpp"

#include "resources/resources.hpp"
#include "tools/renderers/utils/Rectangle.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    Rectangle::Rectangle(IRenderer& renderer, Vector3<float> const& pt1, Vector3<float> const& pt2, Vector3<float> const& pt3, Vector3<float> const& pt4)
        : _renderer(renderer),
        _point1(pt1),
        _point2(pt2),
        _point3(pt3),
        _point4(pt4),
        _point1Color(1, 1, 1, 1),
        _point2Color(1, 1, 1, 1),
        _point3Color(1, 1, 1, 1),
        _point4Color(1, 1, 1, 1)
    {
        Rectangle::_InitRender(renderer);
    }

    void Rectangle::Render()
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
        unsigned char indices[] = { 2, 1, 3, 0 };
        Rectangle::_vertexBuffer->SetSubData(0, sizeof(vertices), vertices);

        Rectangle::_vertexBuffer->Bind();
        do
        {
            Rectangle::_shader->BeginPass();
            this->_renderer.DrawElements(sizeof(indices), Renderers::DataType::UnsignedByte, indices, Renderers::DrawingMode::TrianglesStrip);
        } while (Rectangle::_shader->EndPass());
        Rectangle::_vertexBuffer->Unbind();
    }

    IVertexBuffer* Rectangle::_vertexBuffer = 0;
    IShaderProgram* Rectangle::_shader = 0;

    void Rectangle::_InitRender(IRenderer& renderer)
    {
        if (Rectangle::_shader != 0)
            return;
        // TODO: trouver un moyen de gérer ce type de resources (utile avant une connexion à une partie donc pas de ResourceManager)
        std::ifstream t(RESOURCES_DIR "/BaseShaderColor.cgfx");
        std::string shader((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

        Rectangle::_shader = renderer.CreateProgram(shader).release();

        Rectangle::_vertexBuffer = renderer.CreateVertexBuffer().release();
        Rectangle::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3); // position
        Rectangle::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Color, 4); // color
        Rectangle::_vertexBuffer->SetData(4*3*sizeof(float) + 4*4*sizeof(float), 0, Renderers::VertexBufferUsage::Dynamic);
    }

}}}
