#include "tools/precompiled.hpp"

#include "tools/renderers/utils/DeferredShading.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    DeferredShading::DeferredShading(IRenderer& renderer) :
        _renderer(renderer)
    {
    }

    void DeferredShading::_InitBuffers(IRenderer& renderer)
    {
    }

    void DeferredShading::_DeleteBuffers()
    {
    }

}}}

