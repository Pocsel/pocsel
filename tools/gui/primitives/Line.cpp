
#include "Line.hpp"

namespace Tools { namespace Gui { namespace Primitives {
    Renderers::IVertexBuffer* Line::_vertexBuffer = 0;
    Renderers::IShaderProgram* Line::_shader = 0;
    std::function<void(Line&, IRenderer&)> Line::_renderFunc = &Line::_InitRender;

    void Line::_InitRender(Line& line, IRenderer& renderer)
    {
        char const* vShader = 
            "uniform mat4 u_mvp;"
            "varying vec4 v_color;"
            ""
            "void main() {"
            "   v_color  = gl_Color;"
            "   gl_Position = u_mvp * gl_Vertex;"
            "}";
        char const* fShader =
            "varying vec4 v_color;"
            ""
            "void main() {"
            "   gl_FragColor = v_color;"
            "}";
        
        Line::_shader = renderer.CreateProgram(vShader, fShader).release();
        Line::_shader->SetParameterUsage("u_mvp", Renderers::ShaderParameterUsage::ModelViewProjectionMatrix);

        Line::_vertexBuffer = renderer.CreateVertexBuffer().release();

        Line::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3); // position
        Line::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Color, 4); // color
        Line::_vertexBuffer->SetData(2*7*sizeof(float), 0, Renderers::VertexBufferUsage::Dynamic);

        Line::_renderFunc = &Line::_Render;
        Line::_Render(line, renderer);
    }

    void Line::_Render(Line& line, IRenderer& renderer)
    {
        float vertices[2*7] = 
        {
            line._point1.x, line._point1.y, line._point1.z,
            line._point1Color.r, line._point1Color.g, line._point1Color.b, line._point1Color.a,
            line._point2.x, line._point2.y, line._point2.z,
            line._point2Color.r, line._point2Color.g, line._point2Color.b, line._point2Color.a,
        };
        unsigned char indices[2] = { 0, 1 };

        Line::_vertexBuffer->SetSubData(0, 2*7*sizeof(float), vertices);

        Line::_vertexBuffer->Bind();
        Line::_shader->Activate();

        renderer.DrawVertexBuffer(0, 2, Renderers::DrawingMode::Lines);
        Line::_vertexBuffer->Unbind();
    }


    Line::Line(Vector3<float> const& pt1, Vector3<float> const& pt2) :
        _point1(pt1),
        _point2(pt2),
        _point1Color(1, 1, 1, 1),
        _point2Color(0, 0, 0, 1)
    {
    }
}}}