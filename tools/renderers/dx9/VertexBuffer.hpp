#ifndef __TOOLS_RENDERERS_DX9_VERTEXBUFFER_HPP__
#define __TOOLS_RENDERERS_DX9_VERTEXBUFFER_HPP__

#include "tools/renderers/DX9Renderer.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    class VertexBuffer : public IVertexBuffer
    {
    private:
        //enum
        //{
        //    MaxAttributes = 8
        //};

        //struct VertexAttribute
        //{
        //    GLint location;
        //    GLint nbElements;
        //    GLint type;
        //    GLvoid* offset;
        //};

    private:
        DX9Renderer& _renderer;
        IDirect3DVertexBuffer9* _vertexBuffer;
        //VertexAttribute _attributes[MaxAttributes];
        std::size_t _size;

    public:
        VertexBuffer(DX9Renderer& renderer);
        virtual ~VertexBuffer();

        virtual void PushVertexAttribute(DataType::Type type, VertexAttributeUsage::Type usage, Uint32 nbElements);
        virtual void SetData(std::size_t size, void const* data, VertexBufferUsage::Type usage);
        virtual void SetSubData(std::size_t offset, std::size_t size, void const* data);

        virtual void Bind();
        virtual void Unbind();
    };

}}}

#endif
