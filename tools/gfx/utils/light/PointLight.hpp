#pragma once

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/utils/light/ILight.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Light {

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
