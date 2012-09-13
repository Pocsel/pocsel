#pragma once

namespace Hlsl
{
    // mul(worldMatrix, float4(normal, 0.0))
    struct Statement
    {
        std::string statement;
    };

    struct DeviceState
    {
        std::string key;
        std::string value;
    };

    struct SamplerState
    {
        std::list<DeviceState> states;
    };

    typedef boost::variant<Statement, SamplerState> StatementOrSamplerState;

    // float3 normal : NORMAL
    // float4 position : POSITION
    struct Variable
    {
        std::string type;
        std::string name;
        std::string semantic;
        StatementOrSamplerState value;
    };

    // void vertexShader(float4 position : POSITION, float2 texCoord : TEXCOORD) { ... }
    struct Function
    {
        std::string ret;
        std::string name;
        std::list<Variable> arguments;
        std::string semantic;
        std::list<Statement> statements;
    };

    // VertexShader = vertexShader;
    // AlphaBlendEnable = false;
    struct PassStatement
    {
        std::string key;
        std::string value;
    };

    // pass Toto { VertexShader = vertexShader; PixelShader = pixelShader; }
    struct Pass
    {
        std::string name;
        std::list<PassStatement> statements;
    };

    // technique Toto { pass { VertexShader = vertexShader; PixelShader = pixelShader; }}
    struct Technique
    {
        std::string name;
        std::list<Pass> passes;
    };

    typedef boost::variant<Hlsl::Variable, Hlsl::Function, Hlsl::Technique> GlobalStatement;

    struct File
    {
        std::list<GlobalStatement> statements;
    };

    bool ParseStream(std::istream& in, File& file, std::ostream& errors = std::cerr);
}
