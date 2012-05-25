#ifndef __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__
#define __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/texture/ITexture.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class Material
    {
    private:
        float _shininess;
        float _custom1; // Paramètres custom pour les shaders
        float _custom2; // Paramètres custom pour les shaders
        float _custom3; // Paramètres custom pour les shaders

        std::unique_ptr<Texture::ITexture> _diffuse;
        std::unique_ptr<Texture::ITexture> _specularMap;
        std::unique_ptr<Texture::ITexture> _normalMap;


    };

    class DeferredShading
    {
    private:
        IRenderer& _renderer;

    public:
        DeferredShading(IRenderer& renderer);
        ~DeferredShading();

    private:
        static void _InitBuffers(IRenderer& renderer);
        static void _DeleteBuffers();
    };

}}}

#endif
