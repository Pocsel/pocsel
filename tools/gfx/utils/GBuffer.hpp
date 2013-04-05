#pragma once

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/effect/Effect.hpp"
#include "tools/gfx/utils/Image.hpp"

namespace Tools { namespace Gfx { namespace Utils { namespace Material {
    class LuaMaterial;
}}}}

namespace Tools { namespace Gfx { namespace Utils {

    class GBuffer
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IRenderTarget> _gbufferRenderTarget;
        std::unique_ptr<IRenderTarget> _lightRenderTarget;
        std::unique_ptr<IRenderTarget> _finalRenderTarget;

        // Shaders
        Effect::Effect& _combineShader;
        IShaderParameter* _quadModelViewProj;
        IShaderParameter* _diffuseTexture;
        IShaderParameter* _normalsDepthTexture;
        IShaderParameter* _lightTexture;
        IShaderParameter* _specularTexture;
        //IShaderParameter* _materialTexture; // TODO

        std::unique_ptr<IVertexBuffer> _quad;
        glm::mat4x4 _mvp;
        glm::uvec2 _size;

    public:
        GBuffer(IRenderer& renderer, glm::uvec2 const& size, Effect::Effect& combineShader);

        void Resize(glm::uvec2 const& size);
        glm::uvec2 GetSize() { return this->_size; }
        IVertexBuffer& GetQuad() { return *this->_quad; }

        void Bind();
        void Unbind();

        void BeginLighting();
        void EndLighting();

        void Render(Uint64 totalTime, std::list<Material::LuaMaterial*> const& postProcess = std::list<Material::LuaMaterial*>());

        ITexture2D& GetColors() { return this->_gbufferRenderTarget->GetTexture(0); }
        ITexture2D& GetNormalsDepth() { return this->_gbufferRenderTarget->GetTexture(1); }
        ITexture2D& GetInternalDepthBuffer() { return this->_gbufferRenderTarget->GetTexture(2); }
        ITexture2D& GetLighting() { return this->_lightRenderTarget->GetTexture(0); }
        ITexture2D& GetSpecular() { return this->_lightRenderTarget->GetTexture(1); }
        ITexture2D& GetFinal() { return this->_finalRenderTarget->GetTexture(0); }
    };

}}}
