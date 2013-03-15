#include "tools/precompiled.hpp"

#include "tools/gfx/utils/Line.hpp"

namespace Tools { namespace Gfx { namespace Utils {

    Line::Line(IRenderer& renderer)
        : _renderer(renderer)
    {
        this->_vertexBuffer = this->_renderer.CreateVertexBuffer();
        float vertices[9];
        vertices[0] = 0;
        vertices[1] = 0;
        vertices[2] = 0;
        vertices[3] = 0;
        vertices[4] = 0;
        vertices[5] = 0;
        vertices[6] = 1;
        vertices[7] = 1;
        vertices[8] = 1;
        this->_vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Position, 3); // position
        this->_vertexBuffer->SetData(sizeof(vertices), vertices, VertexBufferUsage::Dynamic);
    }

    void Line::Render()
    {
        this->_vertexBuffer->Bind();
        this->_renderer.DrawVertexBuffer(0, 3);//, Renderers::DrawingMode::Lines);
        this->_vertexBuffer->Unbind();
    }

    void Line::Set(glm::vec3 const& start, glm::vec3 const& end)
    {
        float vertices[9];
        vertices[0] = start.x;
        vertices[1] = start.y;
        vertices[2] = start.z;
        vertices[3] = (start.x + end.x) / 2;
        vertices[4] = (start.y + end.y) / 2;
        vertices[5] = (start.z + end.z) / 2;
        vertices[6] = end.x;
        vertices[7] = end.y;
        vertices[8] = end.z;
        this->_vertexBuffer->SetSubData(0, sizeof(vertices), vertices);
    }

}}}
