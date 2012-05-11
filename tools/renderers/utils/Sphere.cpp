#include "tools/precompiled.hpp"

#include "tools/renderers/utils/Sphere.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    namespace {
        static float vertices[] = { 0.0f,0.0f,0.0f, //dummy
            0.0f, -1.0f, 0.0f, 0.723607f, -0.44722f, 0.525725f,
            -0.276388f, -0.44722f, 0.850649f, -0.894426f, -0.447216f, 0.0f,
            -0.276388f, -0.44722f, -0.850649f, 0.723607f, -0.44722f, -0.525725f,
            0.276388f, 0.44722f, 0.850649f, -0.723607f, 0.44722f, 0.525725f,
            -0.723607f, 0.44722f, -0.525725f, 0.276388f, 0.44722f, -0.850649f,
            0.894426f, 0.447216f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.425323f, -0.850654f, 0.309011f, 0.262869f, -0.525738f, 0.809012f,
            -0.162456f, -0.850654f, 0.499995f, 0.425323f, -0.850654f, -0.309011f,
            0.850648f, -0.525736f, 0.0f, -0.688189f, -0.525736f, 0.499997f,
            -0.52573f, -0.850652f, 0.0f, -0.688189f, -0.525736f, -0.499997f,
            -0.162456f, -0.850654f, -0.499995f, 0.262869f, -0.525738f, -0.809012f,
            0.951058f, 0.0f, -0.309013f, 0.951058f, 0.0f, 0.309013f,
            0.587786f, 0.0f, 0.809017f, 0.0f, 0.0f, 1.0f,
            -0.587786f, 0.0f, 0.809017f, -0.951058f, 0.0f, 0.309013f,
            -0.951058f, 0.0f, -0.309013f, -0.587786f, 0.0f, -0.809017f,
            0.0f, 0.0f, -1.0f, 0.587786f, 0.0f, -0.809017f,
            0.688189f, 0.525736f, 0.499997f, -0.262869f, 0.525738f, 0.809012f,
            -0.850648f, 0.525736f, 0.0f, -0.262869f, 0.525738f, -0.809012f,
            0.688189f, 0.525736f, -0.499997f, 0.52573f, 0.850652f, 0.0f,
            0.162456f, 0.850654f, 0.499995f, -0.425323f, 0.850654f, 0.309011f,
            -0.425323f, 0.850654f, -0.309011f, 0.162456f, 0.850654f, -0.499995f,
        };

        static unsigned short indices[] = {
            1, 13, 15,      2, 13, 17,      1, 15, 19,      1, 19, 21,
            1, 21, 16,      2, 17, 24,      3, 14, 26,      4, 18, 28,
            5, 20, 30,      6, 22, 32,      2, 24, 25,      3, 26, 27,
            4, 28, 29,      5, 30, 31,      6, 32, 23,      7, 33, 39,
            8, 34, 40,      9, 35, 41,      10, 36, 42,     11, 37, 38,
            15, 14, 3,      15, 13, 14,     13, 2, 14,      17, 16, 6,
            17, 13, 16,     13, 1, 16,      19, 18, 4,      19, 15, 18,
            15, 3, 18,      21, 20, 5,      21, 19, 20,     19, 4, 20,
            16, 22, 6,      16, 21, 22,     21, 5, 22,      24, 23, 11,
            24, 17, 23,     17, 6, 23,      26, 25, 7,      26, 14, 25,
            14, 2, 25,      28, 27, 8,      28, 18, 27,     18, 3, 27,
            30, 29, 9,      30, 20, 29,     20, 4, 29,      32, 31, 10,
            32, 22, 31,     22, 5, 31,      25, 33, 7,      25, 24, 33,
            24, 11, 33,     27, 34, 8,      27, 26, 34,     26, 7, 34,
            29, 35, 9,      29, 28, 35,     28, 8, 35,      31, 36, 10,
            31, 30, 36,     30, 9, 36,      23, 37, 11,     23, 32, 37,
            32, 10, 37,     39, 38, 12,     39, 33, 38,     33, 11, 38,
            40, 39, 12,     40, 34, 39,     34, 7, 39,      41, 40, 12,
            41, 35, 40,     35, 8, 40,      42, 41, 12,     42, 36, 41,
            36, 9, 41,      38, 42, 12,     38, 37, 42,     37, 10, 42,
        };
    }

    Sphere::Sphere(IRenderer& renderer)
        : _renderer(renderer)
    {
        this->_vertexBuffer = this->_renderer.CreateVertexBuffer();
        this->_vertexBuffer->PushVertexAttribute(DataType::Float, VertexAttributeUsage::Position, 3); // position
        this->_vertexBuffer->SetData(sizeof(vertices), vertices, VertexBufferUsage::Static);

        this->_indexBuffer = this->_renderer.CreateIndexBuffer();
        this->_indexBuffer->SetData(DataType::UnsignedShort, sizeof(indices), indices);
    }

    void Sphere::Render()
    {
        this->_indexBuffer->Bind();
        this->_vertexBuffer->Bind();
        this->_renderer.DrawElements(sizeof(indices) / sizeof(*indices), Renderers::DataType::UnsignedShort);
        this->_vertexBuffer->Unbind();
        this->_indexBuffer->Unbind();
    }

}}}
