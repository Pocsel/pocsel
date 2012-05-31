#ifndef __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__
#define __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/material/Material.hpp"


namespace Tools { namespace Renderers { namespace Utils {

    class DeferredShading
    {
    private:
        typedef std::list<std::pair<Material::Material*, std::function<void()>>> _MeshList;

        IRenderer& _renderer;
        std::map<IShaderProgram*, std::list<Material::Material*>> _geometryShadersToMaterials;
        std::map<IShaderProgram*, std::list<Material::Material*>> _shadowMapShadersToMaterials;
        _MeshList _geometryMeshes;
        _MeshList _shadowMapMeshes;

    public:
        DeferredShading(IRenderer& renderer);

        void RenderGeometry(Material::Material& material, std::function<void()> const& render)
        {
            this->_geometryMeshes.push_back(std::make_pair(&material, render));
        }
        void RenderShadowMap(Material::Material& material, std::function<void()> const& render)
        {
            this->_shadowMapMeshes.push_back(std::make_pair(&material, render));
        }
        void RenderGeometry() { this->_Render(this->_geometryMeshes); }
        void RenderShadowMap() { this->_Render(this->_shadowMapMeshes); }

        std::unique_ptr<Material::Material, std::function<void(Material::Material*)>> GetMaterial(Lua::Ref const& material, IShaderProgram& geometry, IShaderProgram& shadowMap);

    private:
        void _FreeMaterial(Material::Material* material);
        static void _Render(_MeshList& meshes);
        static void _RenderMeshes(_MeshList::iterator it, _MeshList::iterator ite);
    };

}}}

#endif
