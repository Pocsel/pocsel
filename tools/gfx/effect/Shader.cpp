#include "tools/precompiled.hpp"

#include "tools/ByteArray.hpp"
#include "tools/gfx/effect/Shader.hpp"
#include "tools/gfx/effect/ShaderSerializer.hpp"

namespace Tools { namespace Gfx { namespace Effect {

    CompleteShader::CompleteShader(ByteArray& stream)
    {
        stream.Read(*this);
        this->directX.renderState.Load(this->directX);
        this->openGL.renderState.Load(this->openGL);
    }

}}}
