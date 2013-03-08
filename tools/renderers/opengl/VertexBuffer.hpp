#ifndef __TOOLS_RENDERERS_OPENGL_VERTEXBUFFER_HPP__
#define __TOOLS_RENDERERS_OPENGL_VERTEXBUFFER_HPP__

#include "tools/renderers/GLRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class VertexBuffer : public IVertexBuffer
    {
    public:
        enum
        {
            MaxAttributes = 8
        };

        struct VertexAttribute
        {
            VertexAttributeUsage::Type location;
            GLint nbElements;
            GLint type;
            GLvoid* offset;
            GLuint size;
        };

    private:
        GLRenderer& _renderer;
        GLuint _id;
        GLuint _stride;

        int _nbAttrib;
        VertexAttribute _attributes[MaxAttributes];

    public:
        VertexBuffer(GLRenderer& renderer);
        virtual ~VertexBuffer();

        virtual void PushVertexAttribute(DataType::Type type, VertexAttributeUsage::Type usage, Uint32 nbElements);
        virtual void SetData(std::size_t size, void const* data, VertexBufferUsage::Type usage);
        virtual void SetSubData(std::size_t offset, std::size_t size, void const* data);

        virtual void Bind();
        virtual void Unbind();

        int GetAttributesCount() const { return this->_nbAttrib; }
        VertexAttribute const* GetAttributes() const { return this->_attributes; }
    };

}}}

#endif
