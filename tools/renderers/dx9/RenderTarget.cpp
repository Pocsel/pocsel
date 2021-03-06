#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/renderers/DX9Renderer.hpp"
#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/RenderTarget.hpp"
#include "tools/renderers/dx9/Texture2D.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    RenderTarget::RenderTarget(DX9Renderer& renderer, glm::uvec2 const& size) :
        _renderer(renderer),
        _size(size)
    {
        this->OnResetDevice();
    }

    RenderTarget::RenderTarget(DX9Renderer& renderer, glm::uvec2 const& size, IDirect3DSurface9* color, IDirect3DSurface9* depthBuffer) :
        _renderer(renderer),
        _size(size),
        _depthBuffer(depthBuffer)
    {
        this->_surfaces.emplace_back(color);
    }

    RenderTarget::~RenderTarget()
    {
        this->_renderer.Unregister(*this);
        //for (auto it = this->_surfaces.begin(), ite = this->_surfaces.end(); it != ite; ++it)
        //    Tools::debug << "Release surface: " << (*it)->Release() << std::endl;
        //this->_surfaces.clear();
        //this->_textures.clear();
        //if (this->_depthBuffer != 0)
        //    Tools::debug << "Release depth buffer: " << this->_depthBuffer->Release() << std::endl;
    }

    void RenderTarget::OnLostDevice()
    {
        //for (auto it = this->_surfaces.begin(), ite = this->_surfaces.end(); it != ite; ++it)
        //    Tools::debug << "Release surface: " << (*it)->Release() << std::endl;
        this->_surfaces.clear();
        this->_textures.clear();
        //if (this->_depthBuffer != 0)
        //    Tools::debug << "Release depth buffer: " << this->_depthBuffer->Release() << std::endl;
        this->_depthBuffer = nullptr;
    }

    void RenderTarget::OnResetDevice()
    {
        this->OnLostDevice();
        for (auto it = this->_targets.begin(), ite = this->_targets.end(); it != ite; ++it)
            this->_PushRenderTarget(it->first, it->second);
    }

    int RenderTarget::PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage)
    {
        int id = (int)this->_targets.size();
        this->_targets.push_back(std::make_pair(format, usage));
        this->_PushRenderTarget(format, usage);
        return id;
    }

    void RenderTarget::Bind()
    {
        size_t i;
        for (i = 0; i < this->_surfaces.size(); ++i)
            this->_renderer.GetDevice()->SetRenderTarget((DWORD)i, this->_surfaces[i].get());
        for (; i < DX9Renderer::MaxRenderTargets; ++i)
            this->_renderer.GetDevice()->SetRenderTarget((DWORD)i, 0);
        if (this->_depthBuffer != nullptr)
            this->_renderer.GetDevice()->SetDepthStencilSurface(this->_depthBuffer.get());
    }

    void RenderTarget::Resize(glm::uvec2 const& newSize)
    {
        this->_size = newSize;
        this->OnResetDevice();
    }

    void RenderTarget::_PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage)
    {
        IDirect3DTexture9* tex;
        DXCHECKERROR(D3DXCreateTexture(
            this->_renderer.GetDevice(),
            this->_size.x, this->_size.y, 1,
            usage == RenderTargetUsage::Color ? D3DUSAGE_RENDERTARGET : D3DUSAGE_DEPTHSTENCIL,
            GetFormat(format),
            D3DPOOL_DEFAULT,
            &tex));
        this->_textures.push_back(std::unique_ptr<ITexture2D>(new Texture2D(this->_renderer, tex)));

        IDirect3DSurface9* surface;
        DXCHECKERROR(tex->GetSurfaceLevel(0, &surface));
        if (usage == RenderTargetUsage::Color)
            this->_surfaces.emplace_back(surface);
        else
            this->_depthBuffer.reset(surface);
    }

}}}

#endif