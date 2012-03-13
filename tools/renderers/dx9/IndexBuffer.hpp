#ifndef __TOOLS_RENDERERS_DX9_INDEXBUFFER_HPP__
#define __TOOLS_RENDERERS_DX9_INDEXBUFFER_HPP__

#include "tools/renderers/DX9Renderer.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    class IndexBuffer : public IIndexBuffer
    {
    private:
        DX9Renderer& _renderer;

    public:
        IndexBuffer(DX9Renderer& renderer);
        virtual ~IndexBuffer();

        virtual void SetData(std::size_t size, void const* data);
        virtual void SetSubData(std::size_t offset, std::size_t size, void const* data);

        virtual void Bind();
        virtual void Unbind();
    };

}}}

#endif
