#ifndef __TOOLS_PRIMITIVES_IPRIMITIVE_HPP__
#define __TOOLS_PRIMITIVES_IPRIMITIVE_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Gui { namespace Primitives {

    class IPrimitive
    {
    public:
        virtual ~IPrimitive() {}

        virtual void Render(IRenderer& renderer) = 0;
    };

}}}

#endif
