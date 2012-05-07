#include "tools/precompiled.hpp"

#include "tools/renderers/utils/DirectionnalLight.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    DirectionnalLight::DirectionnalLight(
        IShaderParameter& direction,
        IShaderParameter& diffuseColor,
        IShaderParameter& specularColor) :
        _direction(direction),
        _diffuseColor(diffuseColor),
        _specularColor(specularColor)
    {
    }

    void DirectionnalLight::SetParameters()
    {
        this->_direction.Set(this->direction);
        this->_diffuseColor.Set(this->diffuseColor);
        this->_specularColor.Set(this->specularColor);
    }

}}}
