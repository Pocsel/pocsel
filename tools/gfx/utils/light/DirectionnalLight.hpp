#pragma once

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/utils/light/ILight.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Light {

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

}}}}
