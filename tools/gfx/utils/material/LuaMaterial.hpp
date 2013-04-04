#pragma once

#include <luasel/Luasel.hpp>

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/utils/material/Material.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Material {

    class LuaMaterial
    {
    private:
        Material* _material;
        Luasel::Ref _self;

    public:
        LuaMaterial(IRenderer& renderer, Luasel::Ref const& registedMaterial, std::function<GfxEffect&(std::string const&)> const& loadShader, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture);
        explicit LuaMaterial(LuaMaterial const& material);
        explicit LuaMaterial(Luasel::Ref const& material);

        Luasel::Ref const& GetLuaSelf() const { return this->_self; }
        Material& GetMaterial() { return *this->_material; }
    };

}}}}
