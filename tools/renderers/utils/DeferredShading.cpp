#include "tools/precompiled.hpp"

#include "tools/renderers/utils/DeferredShading.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    DeferredShading::DeferredShading(IRenderer& renderer) :
        _renderer(renderer)
    {
    }

    std::unique_ptr<Material::Material, std::function<void(Material::Material*)>> DeferredShading::GetMaterial(Lua::Ref const& material, IShaderProgram& geometry, IShaderProgram& shadowMap)
    {
        std::unique_ptr<Material::Material, std::function<void(Material::Material*)>> ptr(
            new Material::Material(this->_renderer, material, geometry, shadowMap),
            std::bind(&DeferredShading::_FreeMaterial, this, std::placeholders::_1));

        this->_geometryShadersToMaterials[&geometry].push_back(ptr.get());
        this->_shadowMapShadersToMaterials[&shadowMap].push_back(ptr.get());

        return ptr;
    }

    void DeferredShading::_FreeMaterial(Material::Material* material)
    {
        auto& geometry = material->GetGeometryShader();
        auto& shadowMap = material->GetShadowMapShader();

        this->_geometryShadersToMaterials[&geometry].remove(material);
        this->_shadowMapShadersToMaterials[&shadowMap].remove(material);

        Tools::Delete(material);
    }

}}}

