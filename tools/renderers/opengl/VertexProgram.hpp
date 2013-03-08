#pragma once

#include "tools/renderers/GLRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class VertexProgram : public IVertexProgram
    {
    private:
        Uint32 _shader;

    public:
        VertexProgram(VertexProgram&& prg) : _shader(prg._shader) { prg._shader = 0; }
        explicit VertexProgram(std::string const& code);
        virtual ~VertexProgram();

        Uint32 GetShader() const { return this->_shader; }

    private:
        VertexProgram(VertexProgram const&);
        VertexProgram& operator =(VertexProgram const&);
    };

}}}
