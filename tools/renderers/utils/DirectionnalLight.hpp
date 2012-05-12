#ifndef __TOOLS_RENDERERS_UTILS_DIRECTIONNALLIGHT_HPP__
#define __TOOLS_RENDERERS_UTILS_DIRECTIONNALLIGHT_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/ILight.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class DirectionnalLight : public ILight
    {
    public:
        glm::vec3 direction;
        glm::vec3 ambientColor;
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
    private:
        IShaderParameter& _direction;
        IShaderParameter& _ambientColor;
        IShaderParameter& _diffuseColor;
        IShaderParameter& _specularColor;

    public:
        DirectionnalLight(
            IShaderParameter& direction,
            IShaderParameter& ambientColor,
            IShaderParameter& diffuseColor,
            IShaderParameter& specularColor);

        void SetParameters() const;
    };

}}}

#endif
