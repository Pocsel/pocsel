#ifndef __TOOLS_RENDERERS_UTILS_SPHERE_HPP__
#define __TOOLS_RENDERERS_UTILS_SPHERE_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

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

#endif
