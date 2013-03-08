#include "tools/precompiled.hpp"

#include "tools/ByteArray.hpp"
#include "tools/renderers/effect/Shader.hpp"
#include "tools/renderers/effect/ShaderSerializer.hpp"

namespace Tools { namespace Renderers { namespace Effect {

    CompleteShader::CompleteShader(ByteArray& stream)
    {
        stream.Read(*this);
    }

}}}
