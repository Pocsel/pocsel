#pragma once

#include "sdk/compiler-shader/HlslParser.hpp"

namespace Hlsl {

    std::string HlslFileToGlsl(File const& file, std::string const& source);
    std::string HlslFileToHlsl(File const& file, std::string const& source);

}
