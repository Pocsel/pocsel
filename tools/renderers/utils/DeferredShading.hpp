#ifndef __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__
#define __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class DeferredShading
    {
    private:
        IRenderer& _renderer;

    public:
        DeferredShading(IRenderer& renderer);
        ~DeferredShading();

    private:
        static void _InitBuffers(IRenderer& renderer);
        static void _DeleteBuffers();
    };

}}}

#endif
