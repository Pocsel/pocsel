#include "tools/precompiled.hpp"

#include "tools/gfx/utils/light/PointLight.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Light {

    PointLight::PointLight(
        IShaderParameter& position,
        IShaderParameter& range,
        IShaderParameter& diffuseColor,
        IShaderParameter& specularColor) :
        _position(position),
        _range(range),
        _diffuseColor(diffuseColor),
        _specularColor(specularColor)
    {
    }

    void PointLight::SetParameters() const
    {
        this->_position.Set(this->position);
        this->_range.Set(this->range);
        this->_diffuseColor.Set(this->diffuseColor);
        this->_specularColor.Set(this->specularColor);
    }

}}}}
