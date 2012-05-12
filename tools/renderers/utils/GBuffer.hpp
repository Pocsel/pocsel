#ifndef __TOOLS_RENDERERS_UTILS_GBUFFER_HPP__
#define __TOOLS_RENDERERS_UTILS_GBUFFER_HPP__

#include "tools/IRenderer.hpp"
#include "tools/renderers/utils/Image.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class GBuffer
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IRenderTarget> _gbufferRenderTarget;
        std::unique_ptr<IRenderTarget> _lightRenderTarget;

        // Shaders
        IShaderProgram& _combineShader;
        std::unique_ptr<IShaderParameter> _quadModelViewProj;
        std::unique_ptr<IShaderParameter> _diffuseTexture;
        //std::unique_ptr<IShaderParameter> _normalsDepthTexture;
        std::unique_ptr<IShaderParameter> _lightTexture;
        std::unique_ptr<IShaderParameter> _specularTexture;
        //std::unique_ptr<IShaderParameter> _materialTexture; // TODO

        Image _quad;
        glm::mat4x4 _mvp;

    public:
        GBuffer(IRenderer& renderer, glm::uvec2 const& size, IShaderProgram& combineShader);

        void Resize(glm::uvec2 const& size);

        void Bind();
        void Unbind();

        void BeginLighting();
        void EndLighting();

        void Render();

        ITexture2D& GetColors() { return this->_gbufferRenderTarget->GetTexture(0); }
        ITexture2D& GetNormalsDepth() { return this->_gbufferRenderTarget->GetTexture(1); }
        ITexture2D& GetInternalDepthBuffer() { return this->_gbufferRenderTarget->GetTexture(2); }
        ITexture2D& GetLighting() { return this->_lightRenderTarget->GetTexture(0); }
        ITexture2D& GetSpecular() { return this->_lightRenderTarget->GetTexture(1); }
    };

}}}

#endif
