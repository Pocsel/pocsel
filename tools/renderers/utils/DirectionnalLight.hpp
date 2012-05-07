#ifndef __TOOLS_RENDERERS_UTILS_DIRECTIONNALLIGHT_HPP__
#define __TOOLS_RENDERERS_UTILS_DIRECTIONNALLIGHT_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/ILight.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class DirectionnalLight : public ILight
    {
    public:
        glm::vec3 direction;
        glm::vec3 diffuseColor;
        float diffusePower;
        glm::vec3 specularColor;
        float specularPower;
    private:
        IShaderParameter& _direction;
        IShaderParameter& _diffuseColor;
        IShaderParameter& _diffusePower;
        IShaderParameter& _specularColor;
        IShaderParameter& _specularPower;

    public:
        DirectionnalLight(
            IShaderParameter& direction,
            IShaderParameter& diffuseColor,
            IShaderParameter& diffusePower,
            IShaderParameter& specularColor,
            IShaderParameter& specularPower);

        void SetParameters();
    };

}}}

#endif
