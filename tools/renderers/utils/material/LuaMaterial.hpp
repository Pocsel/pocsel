#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_MATERIAL_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_MATERIAL_HPP__

#include "tools/IRenderer.hpp"
#include "tools/lua/Function.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {
    class Material;
}}}}

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    class LuaMaterial
    {
    private:
        Material _material;

    public:
        LuaMaterial();


    };

}}}}

#endif
