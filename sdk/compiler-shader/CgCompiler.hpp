#pragma once

#include "sdk/compiler-shader/HlslParser.hpp"

namespace Hlsl {

    namespace Type {
        enum Type
        {
            Sampler2D,
            Boolean,
            Float,
            Float2,
            Float3,
            Float3x3,
            Float4,
            Float4x4,
        };
    }
    namespace Profile {
        enum Type
        {
            Hlsl,
            Glsl,
            Arb
        };
    }

    struct Sampler
    {
        std::map<std::string, std::string> states;
    };

    struct BaseParameter
    {
        Type::Type type;
        std::string directX;
        std::string openGL;
    };

    struct UniformParameter : public BaseParameter
    {
        boost::variant<bool, Sampler, std::array<float, 16>, Nil> value;
    };

    struct Shader
    {
        std::string source;
        std::map<std::string, UniformParameter> uniforms;
        std::map<std::string, BaseParameter> attributes;
        std::map<std::string, std::string> deviceStates;
        std::string hlslVertex;
        std::string hlslPixel;
        std::string glslVertex;
        std::string glslPixel;
    };

    std::pair<std::string, std::string> HlslFileToGlsl(File const& file, std::string const& source);
    std::pair<std::string, std::string> HlslFileToHlsl(File const& file, std::string const& source);
    Shader HlslFileToShader(File const& file, std::string const& source);
    void SerializeShader(Shader const& shader, std::ostream& out);

}
