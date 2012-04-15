#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/renderers/DX9Renderer.hpp"
#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/RenderTarget.hpp"
#include "tools/renderers/dx9/Texture2D.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    RenderTarget::RenderTarget(DX9Renderer& renderer, glm::uvec2 const& size) :
        _renderer(renderer),
        _texture(0),
        _size(size)
    {
        this->OnResetDevice();
    }

    RenderTarget::~RenderTarget()
    {
        this->_renderer.Unregister(*this);
        Delete(this->_texture);
        if (this->_surface != 0)
            this->_surface->Release();
    }

    void RenderTarget::OnLostDevice()
    {
        delete this->_texture;
        this->_texture = 0;
        this->_surface->Release();
        this->_surface = 0;
    }

    void RenderTarget::OnResetDevice()
    {
        IDirect3DTexture9* tex;
        DXCHECKERROR(D3DXCreateTexture(this->_renderer.GetDevice(), this->_size.w, this->_size.h, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &tex));
        this->_texture = new Texture2D(this->_renderer, tex);
        DXCHECKERROR(tex->GetSurfaceLevel(0, &this->_surface));
    }

    void RenderTarget::Bind()
    {
        this->_renderer.GetDevice()->SetRenderTarget(0, this->_surface);
    }

}}}

#endif