#pragma once

namespace Hlsl
{
    struct Nil {};

    // mul(worldMatrix, float4(normal, 0.0))
    struct Statement
    {
        std::string statement;
    };

    struct CodeBlock;
    typedef boost::variant<Statement, CodeBlock> StatementOrCodeBlock;
    struct CodeBlock
    {
        std::list<StatementOrCodeBlock> statements;
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

    struct Structure
    {
        std::string name;
        std::list<Variable> members;
    };

    // void vertexShader(float4 position : POSITION, float2 texCoord : TEXCOORD) { ... }
    struct Function
    {
        std::string ret;
        std::string name;
        std::list<Variable> arguments;
        std::string semantic;
        CodeBlock code;
    };

    // AlphaBlendEnable = false;
    struct PassStatement
    {
        std::string key;
        std::string value;
    };

    // VertexShader = compile vs_2_1 vertexShader();
    struct CompileStatement
    {
        enum ShaderType
        {
            VertexShader,
            PixelShader
        } type;
        std::string entry;
        std::string profile;
    };

    typedef boost::variant<PassStatement, CompileStatement> PassOrCompileStatement;
    // pass Toto { VertexShader = vertexShader; PixelShader = pixelShader; }
    struct Pass
    {
        std::string name;
        std::list<PassOrCompileStatement> statements;
    };

    // technique Toto { pass { VertexShader = vertexShader; PixelShader = pixelShader; }}
    struct Technique
    {
        std::string name;
        std::list<Pass> passes;
    };

    typedef boost::variant<Hlsl::Variable, Hlsl::Function, Hlsl::Technique, Hlsl::Structure, Hlsl::Nil> GlobalStatement;

    struct File
    {
        std::list<GlobalStatement> statements;
    };

    bool ParseStream(std::istream& in, File& file, std::ostream& errors = std::cerr);
}
