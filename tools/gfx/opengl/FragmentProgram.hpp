#pragma once

#include "tools/gfx/GLRenderer.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    class FragmentProgram : public IFragmentProgram
    {
    private:
        Uint32 _shader;

    public:
        FragmentProgram(FragmentProgram&& prg) : _shader(prg._shader) { prg._shader = 0; }
        explicit FragmentProgram(std::string const& code);
        virtual ~FragmentProgram();

        Uint32 GetShader() const { return this->_shader; }

    private:
        FragmentProgram(FragmentProgram const&);
        FragmentProgram& operator =(FragmentProgram const&);
    };

}}}
