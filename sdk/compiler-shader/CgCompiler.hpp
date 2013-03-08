#pragma once

#include "sdk/compiler-shader/HlslParser.hpp"

namespace Tools {
    class ByteArray;
}

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
    namespace Semantic {
        enum Type
        {
            // *** Uniforms
            UniformFirst = 0x000,
            // Matrices
            World,
            View,
            Projection,
            WorldView,
            ViewProjection,
            WorldViewProjection,
            // Inverse
            WorldInverse,
            ViewInverse,
            ProjectionInverse,
            WorldViewInverse,
            ViewProjectionInverse,
            WorldViewProjectionInverse,
            // Inverse transpose
            WorldInverseTranspose,
            ViewInverseTranspose,
            ProjectionInverseTranspose,
            WorldViewInverseTranspose,
            ViewProjectionInverseTranspose,
            WorldViewProjectionInverseTranspose,
            UniformLast,

            // *** Attributes
            AttributeFirst = 0x100,
            Position,
            Normal,
            Color,
            TexCoord0,
            TexCoord1,
            TexCoord2,
            TexCoord3,
            TexCoord4,
            AttributeLast,

            // *** Others
            NoSemantic = 0xFFF,
        };
    }

    struct Sampler
    {
        std::map<std::string, std::string> states;
    };

    struct BaseParameter
    {
        Semantic::Type semantic;
        Type::Type type;
        std::string name;
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
        std::string vertex;
        std::string pixel;
    };

    struct CompleteShader
    {
        Shader glsl;
        Shader hlsl;
    };

    Shader HlslFileToShader(File const& file, std::string const& source, Profile::Type profile);
    CompleteShader HlslFileToShader(File const& fileGL, std::string const& sourceGL, File const& fileDX, std::string const& sourceDX);
    void SerializeShader(Shader const& shader, Tools::ByteArray& bin);
    void SerializeShader(CompleteShader const& shader, std::ostream& out);

}
