#pragma once

#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace Utils {

    class Cube
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IVertexBuffer> _vertexBuffer;

    public:
        Cube(IRenderer& renderer);

        void Render();
    };

}}}
