#ifndef __TOOLS_RENDERERS_UTILS_LINE_HPP__
#define __TOOLS_RENDERERS_UTILS_LINE_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class Line
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IVertexBuffer> _vertexBuffer;

    public:
        Line(IRenderer& renderer, glm::vec3 const& start, glm::vec3 const& end);

        void Render();
    };

}}}

#endif
