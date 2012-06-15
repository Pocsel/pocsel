#ifndef __TOOLS_RENDERERS_UTILS_MATERIAL_LUAMATERIAL_HPP__
#define __TOOLS_RENDERERS_UTILS_MATERIAL_LUAMATERIAL_HPP__

#include "tools/IRenderer.hpp"
#include "tools/lua/Function.hpp"
#include "tools/renderers/utils/material/Material.hpp"

namespace Tools { namespace Renderers { namespace Utils { namespace Material {

    class LuaMaterial
    {
    private:
        Material* _material;
        Lua::Ref _self;

    public:
        LuaMaterial(IRenderer& renderer, Lua::Ref const& registedMaterial, std::function<IShaderProgram&(std::string const&)> const& loadShader, std::function<std::unique_ptr<Texture::ITexture>(std::string const&)> loadTexture);
        explicit LuaMaterial(LuaMaterial const& material);
        explicit LuaMaterial(Lua::Ref const& material);

        Lua::Ref const& GetLuaSelf() const { return this->_self; }
        Material& GetMaterial() { return *this->_material; }
    };

}}}}

#endif
