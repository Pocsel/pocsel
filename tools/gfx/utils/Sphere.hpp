#pragma once

#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace Utils {

    class Sphere
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IVertexBuffer> _vertexBuffer;
        std::unique_ptr<IIndexBuffer> _indexBuffer;

    public:
        Sphere(IRenderer& renderer);

        void Render();
    };

}}}
