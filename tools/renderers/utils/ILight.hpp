#ifndef __TOOLS_RENDERERS_UTILS_ILIGHT_HPP__
#define __TOOLS_RENDERERS_UTILS_ILIGHT_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class ILight
    {
    public:
        ~ILight() {}

        virtual void Render(IRenderer& renderer) = 0;
    };

}}}

#endif
