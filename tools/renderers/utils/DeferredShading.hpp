#ifndef __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__
#define __TOOLS_RENDERERS_UTILS_DEFERREDSHADING_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class Material
    {
    public:
        float shininess;
        float custom1; // Paramètres custom pour les shaders
        float custom2; // Paramètres custom pour les shaders
        float custom3; // Paramètres custom pour les shaders

    };

    class DeferredShading
    {
    private:
        IRenderer& _renderer;

    public:
        DeferredShading(IRenderer& renderer);
        ~DeferredShading();

        void Render(IShaderParameter& textureParameter, ITexture2D& texture);

    private:
        static void _InitBuffers(IRenderer& renderer);
        static void _DeleteBuffers();
    };

}}}

#endif
