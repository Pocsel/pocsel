#ifndef __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__
#define __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/material/Material.hpp"


namespace Tools { namespace Renderers { namespace Utils {

    class DeferredShading
    {
    private:
        IRenderer& _renderer;
        std::map<IShaderProgram*, std::list<Material::Material*>> _geometryShadersToMaterials;
        std::map<IShaderProgram*, std::list<Material::Material*>> _shadowMapShadersToMaterials;

    public:
        DeferredShading(IRenderer& renderer);
        ~DeferredShading();

        std::unique_ptr<Material::Material, std::function<void(Material::Material*)>> GetMaterial(Lua::Ref const& material, IShaderProgram& geometry, IShaderProgram& shadowMap);

    private:
        void _FreeMaterial(Material::Material* material);
    };

}}}

#endif
