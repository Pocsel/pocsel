#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_LUAMATERIAL_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_LUAMATERIAL_HPP__

#include <luasel/Luasel.hpp>

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/material/Material.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    class LuaMaterial
    {
    private:
        Material* _material;
        Luasel::Ref _self;

    public:
        LuaMaterial(IRenderer& renderer, Luasel::Ref const& registedMaterial, std::function<IShaderProgram&(std::string const&)> const& loadShader, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture);
        explicit LuaMaterial(LuaMaterial const& material);
        explicit LuaMaterial(Luasel::Ref const& material);

        Luasel::Ref const& GetLuaSelf() const { return this->_self; }
        Material& GetMaterial() { return *this->_material; }
    };

}}}}

#endif
