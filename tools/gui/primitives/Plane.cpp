
#include "Plane.hpp"

namespace Tools { namespace Gui { namespace Primitives {
    Renderers::IVertexBuffer* Plane::_vertexBuffer = 0;
    Renderers::IShaderProgram* Plane::_shader = 0;
    void (*Plane::_renderFunc)(Plane&, IRenderer&) = &Plane::_InitRender;

    void Plane::_InitRender(Plane& plane, IRenderer& renderer)
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
        
        Plane::_shader = renderer.CreateProgram(vShader, fShader).release();
        Plane::_shader->SetParameterUsage("u_mvp", Renderers::ShaderParameterUsage::ModelViewProjectionMatrix);

        Plane::_vertexBuffer = renderer.CreateVertexBuffer().release();

        Plane::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3); // position
        Plane::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Color, 4); // color
        Plane::_vertexBuffer->SetData(4*3*sizeof(float) + 4*4*sizeof(float), 0, Renderers::VertexBufferUsage::Dynamic);

        Plane::_renderFunc = &Plane::_Render;
        Plane::_Render(plane, renderer);
    }

    void Plane::_Render(Plane& plane, IRenderer& renderer)
    {
        float vertices[] = 
        {
            plane._point1.x, plane._point1.y, plane._point1.z,
            plane._point1Color.r, plane._point1Color.g, plane._point1Color.b, plane._point1Color.a,
            plane._point2.x, plane._point2.y, plane._point2.z,
            plane._point2Color.r, plane._point2Color.g, plane._point2Color.b, plane._point2Color.a,
            plane._point3.x, plane._point3.y, plane._point3.z,
            plane._point3Color.r, plane._point3Color.g, plane._point3Color.b, plane._point3Color.a,
            plane._point4.x, plane._point4.y, plane._point4.z,
            plane._point4Color.r, plane._point4Color.g, plane._point4Color.b, plane._point4Color.a
        };
        unsigned char indices[] = { 2, 1, 3, 0 };

        Plane::_vertexBuffer->SetSubData(0, sizeof(vertices), vertices);

        Plane::_vertexBuffer->Bind();
        Plane::_shader->Activate();
        renderer.DrawElements(sizeof(indices), Renderers::DataType::UnsignedByte, indices, Renderers::DrawingMode::TrianglesStrip);
        Plane::_vertexBuffer->Unbind();
    }


    Plane::Plane(Vector3<float> const& pt1, Vector3<float> const& pt2, Vector3<float> const& pt3, Vector3<float> const& pt4) :
        _point1(pt1),
        _point2(pt2),
        _point3(pt3),
        _point4(pt4),
        _point1Color(1, 1, 1, 1),
        _point2Color(0.66f, 0.66f, 0.66f, 1),
        _point3Color(0, 0, 0, 1),
        _point4Color(0.66f, 0.66f, 0.66f, 1)
    {
    }
}}}