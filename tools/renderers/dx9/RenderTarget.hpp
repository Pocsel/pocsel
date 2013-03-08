#ifndef __TOOLS_RENDERERS_DX9_RENDERTARGET_HPP__
#define __TOOLS_RENDERERS_DX9_RENDERTARGET_HPP__

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
        std::vector<ComPtr<IDirect3DSurface9>> _surfaces;
        ComPtr<IDirect3DSurface9> _depthBuffer;

        std::vector<std::pair<PixelFormat::Type, RenderTargetUsage::Type>> _targets;

    public:
        RenderTarget(DX9Renderer& renderer, glm::uvec2 const& size);
        RenderTarget(DX9Renderer& renderer, glm::uvec2 const& size, IDirect3DSurface9* color, IDirect3DSurface9* depthBuffer);
        virtual ~RenderTarget();

        void OnLostDevice();
        void OnResetDevice();
        virtual int PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage);
        virtual void Bind();
        virtual void Resize(glm::uvec2 const& newSize);

        virtual ITexture2D& GetTexture(int idx) { return *(this->_textures[idx].get()); }
        virtual glm::uvec2 const& GetSize() const { return this->_size; }
    private:
        void _PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage);
    };

}}}

#endif
