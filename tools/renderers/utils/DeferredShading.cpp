#include "tools/precompiled.hpp"

#include "tools/renderers/utils/DeferredShading.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    DeferredShading::DeferredShading(IRenderer& renderer) :
        _renderer(renderer)
    {
    }

    void DeferredShading::_Render(Uint64 totalTime, _MeshList& meshes, IShaderProgram& (Material::Material::* getShader)())
    {
        //TODO:
        meshes.sort(
            [&](_MeshList::value_type const& lhs, _MeshList::value_type const& rhs) -> bool
            {
                if (&(lhs.first->*getShader)() >= &(rhs.first->*getShader)())
                    return false;
                auto lhsIt = lhs.first->GetTextures().begin();
                auto lhsIte = lhs.first->GetTextures().end();
                auto rhsIt = rhs.first->GetTextures().begin();
                auto rhsIte = rhs.first->GetTextures().end();
                for (auto it = lhsIt; it != lhsIte; ++it)
                {
                    bool run = false;
                    for (auto i = rhsIt; i != rhsIte; ++i)
                        if (&i->second->GetCurrentTexture() == &it->second->GetCurrentTexture())
                        {
                            run = true;
                            break;
                        }
                    if (!run)
                        return false;
                }
                return true;
            });

        auto beginShaderIt = meshes.begin();
        IShaderProgram* current = &beginShaderIt->first->GetGeometryShader();
        for (auto it = meshes.begin(), ite = meshes.end(); it != ite; ++it)
        {
            it->first->Update(totalTime);
            auto shader = &(it->first->*getShader)();
            if (shader != current)
            {
                DeferredShading::_RenderMeshes(totalTime, beginShaderIt, it, *current);
                beginShaderIt = it;
                current = shader;
            }
        }
        DeferredShading::_RenderMeshes(totalTime, beginShaderIt, meshes.end(), *current);

        meshes.clear();
    }

    void DeferredShading::_RenderMeshes(Uint64 totalTime, _MeshList::iterator beginIt, _MeshList::iterator ite, IShaderProgram& shader)
    {
        std::list<ITexture2D*> bindedTextures;
        do
        {
            shader.BeginPass();
            for (auto it = beginIt; it != ite; ++it)
            {
                // Les textures
                auto const& textures = it->first->GetTextures();
                bindedTextures.remove_if([&](ITexture2D* tex) -> bool
                    {
                        bool remove = true;
                        for (auto itTexture = textures.begin(), iteTexture = textures.end(); itTexture != iteTexture; ++itTexture)
                            remove = remove && &itTexture->second->GetCurrentTexture() != tex;
                        if (remove)
                            tex->Unbind();
                        return remove;
                    });
                for (auto itTexture = textures.begin(), iteTexture = textures.end(); itTexture != iteTexture; ++itTexture)
                {
                    bool binded = false;
                    for (auto itBinded = bindedTextures.begin(), iteBinded = bindedTextures.end(); itBinded != iteBinded; ++itBinded)
                        binded = binded || *itBinded == &itTexture->second->GetCurrentTexture();
                    if (!binded)
                    {
                        itTexture->second->GetCurrentTexture().Bind();
                        bindedTextures.push_back(&itTexture->second->GetCurrentTexture());
                    }
                }

                // Render
                it->first->UpdateParameters(shader, totalTime);
                it->second();
            }
        } while (shader.EndPass());

        // Unbind tout
        for (auto itBinded = bindedTextures.begin(), iteBinded = bindedTextures.end(); itBinded != iteBinded; ++itBinded)
            (*itBinded)->Unbind();
    }

}}}

