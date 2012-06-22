#ifndef __TOOLS_RENDERERS_UTILS_CUBE_HPP__
#define __TOOLS_RENDERERS_UTILS_CUBE_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

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

#endif

