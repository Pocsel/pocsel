
#include "Cube.hpp"

namespace Tools { namespace Gui { namespace Primitives {
    Renderers::IVertexBuffer* Cube::_vertexBuffer = 0;
    Renderers::IVertexBuffer* Cube::_colorBuffer = 0;
    Renderers::IShaderProgram* Cube::_shader = 0;
    void (*Cube::_renderFunc)(Cube&, IRenderer&) = &Cube::_InitRender;

    void Cube::_InitRender(Cube& cube, IRenderer& renderer)
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
        
        Cube::_shader = renderer.CreateProgram(vShader, fShader).release();
        Cube::_shader->SetParameterUsage("u_mvp", Renderers::ShaderParameterUsage::ModelViewProjectionMatrix);

#define CreateQuad(v1, v2, v3, v4, n1, n2, n3) \
    v1.x,v1.y,v1.z, n1,n2,n3, \
    v2.x,v2.y,v2.z, n1,n2,n3, \
    v3.x,v3.y,v3.z, n1,n2,n3, \
    v4.x,v4.y,v4.z, n1,n2,n3

        Vector3f frontUpLeft(-1, 1, 1);
        Vector3f frontUpRight(1, 1, 1);
        Vector3f frontDownLeft(-1, -1, 1);
        Vector3f frontDownRight(1, -1, 1);
        Vector3f backUpLeft(-1, 1, -1);
        Vector3f backUpRight(1, 1, -1);
        Vector3f backDownLeft(-1, -1, -1);
        Vector3f backDownRight(1, -1, -1);

        float vertices[] = {
            CreateQuad(frontUpLeft, frontUpRight, frontDownRight, frontDownLeft, 0, 0, 1),
            CreateQuad(backUpLeft, backUpRight, backDownRight, backDownLeft, 0, 0, -1),

            CreateQuad(frontUpLeft, frontUpRight, backUpRight, backUpLeft, 0, 1, 0),
            CreateQuad(frontDownLeft, frontDownRight, backDownRight, backDownLeft, 0, -1, 0),

            CreateQuad(backUpLeft, backDownLeft, frontDownLeft, frontUpLeft, -1, 0, 0),
            CreateQuad(backUpRight, backDownRight, frontDownRight, frontUpRight, 1, 0, 0),
        };
#undef CreateQuad


        Cube::_vertexBuffer = renderer.CreateVertexBuffer().release();
        Cube::_colorBuffer = renderer.CreateVertexBuffer().release();

        Cube::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Position, 3); // position
        Cube::_vertexBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Normal, 3); // normal
        Cube::_vertexBuffer->SetData(6*4*(3*sizeof(float) + 3*sizeof(float)), vertices, Renderers::VertexBufferUsage::Static);

        Cube::_colorBuffer->PushVertexAttribute(Renderers::DataType::Float, Renderers::VertexAttributeUsage::Color, 4); // color
        Cube::_colorBuffer->SetData(6*4*(4*sizeof(float)), 0, Renderers::VertexBufferUsage::Dynamic);

        Cube::_renderFunc = &Cube::_Render;
        Cube::_Render(cube, renderer);
    }

    void Cube::_Render(Cube& cube, IRenderer& renderer)
    {
        static unsigned char indices[] = {
             1, 0, 2,  2, 0, 3, // front
             5, 6, 4,  6, 7, 4, // back
             9,10, 8, 10,11, 8, // up
            13,12,14, 14,12,15, // down
            17,18,16, 18,19,16, // left
            21,20,22, 22,20,23  // right
        };

        float colors[] = {
            cube._point1Color.r, cube._point1Color.g, cube._point1Color.b, cube._point1Color.a,
            cube._point1Color.r, cube._point1Color.g, cube._point1Color.b, cube._point1Color.a,
            cube._point1Color.r, cube._point1Color.g, cube._point1Color.b, cube._point1Color.a,
            cube._point1Color.r, cube._point1Color.g, cube._point1Color.b, cube._point1Color.a,
            
            cube._point2Color.r, cube._point2Color.g, cube._point2Color.b, cube._point2Color.a,
            cube._point2Color.r, cube._point2Color.g, cube._point2Color.b, cube._point2Color.a,
            cube._point2Color.r, cube._point2Color.g, cube._point2Color.b, cube._point2Color.a,
            cube._point2Color.r, cube._point2Color.g, cube._point2Color.b, cube._point2Color.a,

            cube._point3Color.r, cube._point3Color.g, cube._point3Color.b, cube._point3Color.a,
            cube._point3Color.r, cube._point3Color.g, cube._point3Color.b, cube._point3Color.a,
            cube._point3Color.r, cube._point3Color.g, cube._point3Color.b, cube._point3Color.a,
            cube._point3Color.r, cube._point3Color.g, cube._point3Color.b, cube._point3Color.a,

            cube._point4Color.r, cube._point4Color.g, cube._point4Color.b, cube._point4Color.a,
            cube._point4Color.r, cube._point4Color.g, cube._point4Color.b, cube._point4Color.a,
            cube._point4Color.r, cube._point4Color.g, cube._point4Color.b, cube._point4Color.a,
            cube._point4Color.r, cube._point4Color.g, cube._point4Color.b, cube._point4Color.a,

            cube._point5Color.r, cube._point5Color.g, cube._point5Color.b, cube._point5Color.a,
            cube._point5Color.r, cube._point5Color.g, cube._point5Color.b, cube._point5Color.a,
            cube._point5Color.r, cube._point5Color.g, cube._point5Color.b, cube._point5Color.a,
            cube._point5Color.r, cube._point5Color.g, cube._point5Color.b, cube._point5Color.a,

            cube._point6Color.r, cube._point6Color.g, cube._point6Color.b, cube._point6Color.a,
            cube._point6Color.r, cube._point6Color.g, cube._point6Color.b, cube._point6Color.a,
            cube._point6Color.r, cube._point6Color.g, cube._point6Color.b, cube._point6Color.a,
            cube._point6Color.r, cube._point6Color.g, cube._point6Color.b, cube._point6Color.a
        };

        Cube::_colorBuffer->SetSubData(0, sizeof(colors), colors);

        Cube::_vertexBuffer->Bind();
        Cube::_colorBuffer->Bind();
        Cube::_shader->Activate();
        renderer.DrawElements(sizeof(indices), Renderers::DataType::UnsignedByte, indices);
        Cube::_vertexBuffer->Unbind();
        Cube::_colorBuffer->Unbind();
    }


    Cube::Cube() :
        _point1Color(1, 1, 1, 1),
        _point2Color(1, 0, 0, 1),
        _point3Color(0, 1, 0, 1),
        _point4Color(0, 0, 1, 1),
        _point5Color(0, 1, 1, 1),
        _point6Color(0, 0, 0, 1)
    {
    }
}}}