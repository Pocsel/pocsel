#ifndef __TOOLS_RENDERERS_UTILS_POINTLIGHT_HPP__
#define __TOOLS_RENDERERS_UTILS_POINTLIGHT_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/light/ILight.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Light {

    class PointLight : public ILight
    {
    public:
        glm::vec3 position;
        float range;
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
    private:
        IShaderParameter& _position;
        IShaderParameter& _range;
        IShaderParameter& _diffuseColor;
        IShaderParameter& _specularColor;

    public:
        PointLight(
            IShaderParameter& position,
            IShaderParameter& range,
            IShaderParameter& diffuseColor,
            IShaderParameter& specularColor);

        void SetParameters() const;
    };

}}}}

#endif
