#include "tools/precompiled.hpp"

#include "tools/gfx/utils/Cube.hpp"

namespace Tools { namespace Gfx { namespace Utils {

    namespace {
        static float vertices[] = {
            // front
            -1.0f, 1.0f, -1.0f, // 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, // 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,//  1.0f, 0.0f,

            1.0f, 1.0f, -1.0f,  //0.0f, 1.0f,
            1.0f, -1.0f, -1.0f, // 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, // 1.0f, 1.0f,

            // back
            1.0f, 1.0f, 1.0f,  //1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,//  0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, // 1.0f, 0.0f,

            -1.0f, 1.0f, 1.0f, // 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,//  0.0f, 0.0f,
            1.0f, 1.0f, 1.0f,  //1.0f, 1.0f,

            // left
            -1.0f, 1.0f, 1.0f,  //1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,//  0.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, // 1.0f, 0.0f,

            -1.0f, 1.0f, -1.0f, // 0.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,//  0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f,  //1.0f, 1.0f,

            // right
            1.0f, 1.0f, -1.0f, // 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, // 0.0f, 0.0f,
            1.0f, -1.0f, -1.0f,//  1.0f, 0.0f,

            1.0f, 1.0f, 1.0f,  //0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, // 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f, // 1.0f, 1.0f,

            // bottom
            -1.0f, -1.0f, -1.0f,//  1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,  //0.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, // 1.0f, 0.0f,

            1.0f, -1.0f, -1.0f, // 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f,  //0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,//  1.0f, 1.0f,

            // top
            -1.0f, 1.0f, 1.0f, // 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f, // 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f,//  1.0f, 0.0f,

            1.0f, 1.0f, 1.0f,  //0.0f, 1.0f,
            1.0f, 1.0f, -1.0f, // 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, // 1.0f, 1.0f,
        };
    }

    Cube::Cube(IRenderer& renderer)
        : _renderer(renderer)
    {
        this->_vertexBuffer = this->_renderer.CreateVertexBuffer();
        this->_vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Position, 3); // position
        this->_vertexBuffer->SetData(sizeof(vertices), vertices, VertexBufferUsage::Static);
    }

    void Cube::Render()
    {
        this->_vertexBuffer->Bind();
        this->_renderer.DrawVertexBuffer(0, 6*6);
        this->_vertexBuffer->Unbind();
    }

}}}

