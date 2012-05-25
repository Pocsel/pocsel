#include "tools/precompiled.hpp"

#include "tools/renderers/utils/light/DirectionnalLight.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Light {

    DirectionnalLight::DirectionnalLight(
        IShaderParameter& direction,
        IShaderParameter& ambientColor,
        IShaderParameter& diffuseColor,
        IShaderParameter& specularColor) :
        _direction(direction),
        _ambientColor(ambientColor),
        _diffuseColor(diffuseColor),
        _specularColor(specularColor)
    {
    }

    void DirectionnalLight::SetParameters() const
    {
        this->_direction.Set(this->direction);
        this->_ambientColor.Set(this->ambientColor);
        this->_diffuseColor.Set(this->diffuseColor);
        this->_specularColor.Set(this->specularColor);
    }

}}}}
