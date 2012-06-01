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
        void RenderGeometry(Uint64 totalTime) { this->_Render(totalTime, this->_geometryMeshes, &Material::Material::GetGeometryShader); }
        void RenderShadowMap(Uint64 totalTime) { this->_Render(totalTime, this->_shadowMapMeshes, &Material::Material::GetShadowMapShader); }

    private:
        static void _Render(Uint64 totalTime, _MeshList& meshes, IShaderProgram& (Material::Material::* getShader)());
        static void _RenderMeshes(Uint64 totalTime, _MeshList::iterator it, _MeshList::iterator ite, IShaderProgram& shader);
    };

}}}

#endif
