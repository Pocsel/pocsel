#pragma once

#include "sdk/compiler-shader/HlslParser.hpp"

namespace Hlsl {

    struct GeneratorOptions
    {
        bool removeSemanticAttributes;

        GeneratorOptions() :
            removeSemanticAttributes(false)
        {
        }
    };

    std::string GenerateHlsl(File const& file, GeneratorOptions const& options = GeneratorOptions());

}
