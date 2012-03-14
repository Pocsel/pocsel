#ifndef __TOOLS_RENDERERS_DX9_VERTEXBUFFER_HPP__
#define __TOOLS_RENDERERS_DX9_VERTEXBUFFER_HPP__

namespace Tools { namespace Renderers {
    class DX9Renderer;
}}

namespace Tools { namespace Renderers { namespace DX9 {

    class VertexBuffer : public IVertexBuffer
    {
    public:
        enum
        {
            MaxAttributes = 8
        };

    private:
        DX9Renderer& _renderer;
        IDirect3DVertexBuffer9* _vertexBuffer;
        std::size_t _size;

        D3DVERTEXELEMENT9 _attributes[MaxAttributes + 1];
        LPDIRECT3DVERTEXDECLARATION9 _vertexDeclaration;
        int _nbAttrib;
        UINT _stride;

    public:
        VertexBuffer(DX9Renderer& renderer);
        virtual ~VertexBuffer();

        virtual void PushVertexAttribute(DataType::Type type, VertexAttributeUsage::Type usage, Uint32 nbElements);
        virtual void SetData(std::size_t size, void const* data, VertexBufferUsage::Type usage);
        virtual void SetSubData(std::size_t offset, std::size_t size, void const* data);

        virtual void Bind();
        virtual void Unbind();

        //VertexAttribute const* GetAttributes() const { return this->_attributes; }
        //int GetNbAttributes() const { return this->_nbAttrib; }
        //UINT GetStride() const { return this->_stride; }
        IDirect3DVertexBuffer9* GetVertexBuffer() const { return this->_vertexBuffer; }
    };

}}}

#endif
