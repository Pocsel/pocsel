#include "tools/precompiled.hpp"

#include "tools/renderers/utils/DeferredShading.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    DeferredShading::DeferredShading(IRenderer& renderer) :
        _renderer(renderer)
    {
    }

    void DeferredShading::_Render(Uint64 totalTime, _MeshList& meshes, IShaderProgram& (Material::Material::* getShader)())
    {
        //TODO: Alpha !
        std::sort(meshes.begin(), meshes.end(),
            [&getShader](_MeshList::value_type const& lhs, _MeshList::value_type const& rhs) -> bool
            {
                if (&(std::get<0>(lhs)->*getShader)() >= &(std::get<0>(rhs)->*getShader)())
                    return false;
                auto lhsIt = std::get<0>(lhs)->GetTextures().begin();
                auto lhsIte = std::get<0>(lhs)->GetTextures().end();
                auto rhsIt = std::get<0>(rhs)->GetTextures().begin();
                auto rhsIte = std::get<0>(rhs)->GetTextures().end();
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
                // On affiche du plus près au plus éloigné -- TODO: seulement sans alpha
                return std::get<2>(lhs) < std::get<2>(rhs);
            });

        auto beginShaderIt = meshes.begin();
        if (beginShaderIt == meshes.end())
            return;
        IShaderProgram* current = &std::get<0>(*beginShaderIt)->GetGeometryShader();
        for (auto it = meshes.begin(), ite = meshes.end(); it != ite; ++it)
        {
            std::get<0>(*it)->Update(totalTime);
            auto shader = &(std::get<0>(*it)->*getShader)();
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
                auto const& textures = std::get<0>(*it)->GetTextures();
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
                std::get<0>(*it)->UpdateParameters(shader, totalTime);
                std::get<1>(*it)();
            }
        } while (shader.EndPass());

        // Unbind tout
        for (auto itBinded = bindedTextures.begin(), iteBinded = bindedTextures.end(); itBinded != iteBinded; ++itBinded)
            (*itBinded)->Unbind();
    }

}}}

