#pragma once

#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace Utils {

    class Line
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IVertexBuffer> _vertexBuffer;

    public:
        Line(IRenderer& renderer);

        void Render();

        void Set(glm::vec3 const& start, glm::vec3 const& end);
    };

}}}
