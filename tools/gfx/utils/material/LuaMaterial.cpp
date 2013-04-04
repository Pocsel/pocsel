#include "tools/precompiled.hpp"

#include "tools/gfx/utils/material/LuaMaterial.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Material {

    LuaMaterial::LuaMaterial(IRenderer& renderer, Luasel::Ref const& registeredMaterial, std::function<GfxEffect&(std::string const&)> const& loadShader, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture) :
        _self(registeredMaterial.GetState())
    {
        this->_material = new Material(renderer, registeredMaterial, this->_self, loadShader, loadTexture);
        this->_self.Set("shader", this->_self.GetState().GetInterpreter().MakeMove(std::unique_ptr<Material>(this->_material)));
    }

    LuaMaterial::LuaMaterial(LuaMaterial const& material) :
        _self(material._self.GetState())
    {
        this->_material = new Material(*material._material, this->_self);
        this->_self.Set("shader", this->_self.GetState().GetInterpreter().MakeMove(std::unique_ptr<Material>(this->_material)));
    }

    LuaMaterial::LuaMaterial(Luasel::Ref const& luaRef) :
        _self(luaRef)
    {
        this->_material = luaRef["shader"].Check<std::unique_ptr<Material>*>("LuaMaterial(Luasel::Ref): It's not a material")->get();
    }

}}}}
