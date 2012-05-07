#include "tools/precompiled.hpp"

#include "tools/renderers/utils/DirectionnalLight.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    DirectionnalLight::DirectionnalLight(
        IShaderParameter& direction,
        IShaderParameter& diffuseColor,
        IShaderParameter& diffusePower,
        IShaderParameter& specularColor,
        IShaderParameter& specularPower) :
        _direction(direction),
        _diffuseColor(diffuseColor),
        _diffusePower(diffusePower),
        _specularColor(specularColor),
        _specularPower(specularPower)
    {
    }

    void DirectionnalLight::SetParameters()
    {
        this->_direction.Set(this->direction);
        this->_diffuseColor.Set(this->diffuseColor);
        this->_diffusePower.Set(this->diffusePower);
        this->_specularColor.Set(this->specularColor);
        this->_specularPower.Set(this->specularPower);
    }

}}}
