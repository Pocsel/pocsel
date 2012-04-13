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
        IDirect3DSurface9* _surface;
        Texture2D* _texture;
        glm::uvec2 _size;

    public:
        RenderTarget(DX9Renderer& renderer, glm::uvec2 const& size);
        virtual ~RenderTarget();

        void OnLostDevice();
        void OnResetDevice();
        virtual void Bind();

        virtual ITexture2D& GetTexture() { return *this->_texture; }
    };

}}}

#endif
