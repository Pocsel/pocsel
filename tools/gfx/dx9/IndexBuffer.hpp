#pragma once

#include "tools/gfx/DX9Renderer.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    class IndexBuffer : public IIndexBuffer
    {
    private:
        DX9Renderer& _renderer;
        IDirect3DIndexBuffer9* _indexBuffer;

    public:
        IndexBuffer(DX9Renderer& renderer);
        virtual ~IndexBuffer();

        virtual void SetData(DataType::Type indicesType, std::size_t size, void const* data);
        virtual void SetSubData(std::size_t offset, std::size_t size, void const* data);

        virtual void Bind();
        virtual void Unbind();
    };

}}}
