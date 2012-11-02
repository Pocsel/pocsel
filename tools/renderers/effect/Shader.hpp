#pragma once

namespace Tools { namespace Renderers { namespace Effect {

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

    struct UniformParameter
    {
        Type::Type type;
        std::string name;
        boost::variant<Sampler, std::string> value;
    };

    struct Shader
    {
        std::string source;
        std::map<std::string, UniformParameter> uniforms;
        std::map<std::string, std::string> attributes;
        std::map<std::string, std::string> deviceStates;
        std::string vertex;
        std::string pixel;
    };


    struct CompleteShader
    {
        Shader openGL;
        Shader directX;
    };
}}}