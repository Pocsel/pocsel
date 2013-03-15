#pragma once

#include "tools/gfx/IRenderer.hpp"
#include "tools/lua/Function.hpp"
#include "tools/gfx/utils/material/Material.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Material {

    class LuaMaterial
    {
    private:
        Material* _material;
        Lua::Ref _self;

    public:
        LuaMaterial(IRenderer& renderer, Lua::Ref const& registedMaterial, std::function<GfxEffect&(std::string const&)> const& loadShader, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture);
        explicit LuaMaterial(LuaMaterial const& material);
        explicit LuaMaterial(Lua::Ref const& material);

        Lua::Ref const& GetLuaSelf() const { return this->_self; }
        Material& GetMaterial() { return *this->_material; }
    };

}}}}
