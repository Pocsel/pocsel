#ifndef __TOOLS_RENDERERS_DX9_RENDERTARGET_HPP__
#define __TOOLS_RENDERERS_DX9_RENDERTARGET_HPP__

#include "tools/renderers/dx9/Texture2D.hpp"

namespace Tools { namespace Renderers {
    class DX9Renderer;
}}

namespace Tools { namespace Renderers { namespace DX9 {

    class RenderTarget : public IRenderTarget
    {
    private:
        DX9Renderer& _renderer;
        glm::uvec2 _size;
        std::vector<std::unique_ptr<ITexture2D>> _textures;
        std::vector<IDirect3DSurface9*> _surfaces;
        IDirect3DSurface9* _depthBuffer;

        std::vector<std::pair<PixelFormat::Type, RenderTargetUsage::Type>> _targets;

    public:
        RenderTarget(DX9Renderer& renderer, glm::uvec2 const& size);
        virtual ~RenderTarget();

        void OnLostDevice();
        void OnResetDevice();
        virtual int PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage);
        virtual void Bind();

        virtual ITexture2D& GetTexture(int idx) { return *(this->_textures[idx].get()); }
    private:
        void _PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage);
    };

}}}

#endif
