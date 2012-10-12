#include "precompiled.hpp"

#include "sdk/compiler-shader/CgCompiler.hpp"
#include "sdk/compiler-shader/HlslGenerator.hpp"

namespace Hlsl {

    static CGprofile _GetCGProfile(Profile::Type profile, CompileStatement::ShaderType type)
    {
        switch (profile)
        {
        case Profile::Hlsl: return type == CompileStatement::VertexShader ? CG_PROFILE_VS_3_0 : CG_PROFILE_PS_3_0;
        case Profile::Glsl: return type == CompileStatement::VertexShader ? CG_PROFILE_GLSLV : CG_PROFILE_GLSLF;
        case Profile::Arb: return type == CompileStatement::VertexShader ? CG_PROFILE_ARBVP1 : CG_PROFILE_ARBFP1;
        }
        throw std::invalid_argument("unknown profile");
    }

    static Pass const& _GetFirstPass(File const& file)
    {
        for (auto const& s: file.statements)
            if (s.which() == 2)
                for (auto const& pass: boost::get<Technique>(s).passes)
                    return pass;
        throw std::runtime_error("can't find any pass");
    }

    static CompileStatement _GetFirstShader(Pass const& pass, CompileStatement::ShaderType type)
    {
        for (auto const& s: pass.statements)
            if (s.which() == 1 && boost::get<CompileStatement>(s).type == type)
                return boost::get<CompileStatement>(s);
        throw std::runtime_error(std::string("can't find ") + (type == CompileStatement::PixelShader ? "PixelShader" : "VertexShader") + " in pass " + pass.name);
    };

    static std::pair<std::string, std::string> _Compile(std::string const& vertexShader, std::string const& pixelShader, std::string const& source, Profile::Type profile)
    {
        std::pair<std::string, std::string> result;
        auto ctx = cgCreateContext();

        auto program = cgCreateProgram(ctx, CG_SOURCE, source.c_str(), _GetCGProfile(profile, CompileStatement::VertexShader), vertexShader.c_str(), 0);
        if (program == 0)
        {
            std::cout << "Error " << cgGetError() << ": " << cgGetErrorString(cgGetError()) << std::endl;
            std::cout << cgGetLastListing(ctx) << std::endl;
        }
        result.first = cgGetProgramString(program, CG_COMPILED_PROGRAM);

        program = cgCreateProgram(ctx, CG_SOURCE, source.c_str(), _GetCGProfile(profile, CompileStatement::PixelShader), pixelShader.c_str(), 0);
        if (program == 0)
        {
            std::cout << "Error " << cgGetError() << ": " << cgGetErrorString(cgGetError()) << std::endl;
            std::cout << cgGetLastListing(ctx) << std::endl;
        }
        result.second = cgGetProgramString(program, CG_COMPILED_PROGRAM);

        cgDestroyProgram(program);
        cgDestroyContext(ctx);

        return result;
    }

    std::pair<std::string, std::string> HlslFileToGlsl(File const& file, std::string const& source)
    {
        auto pass = _GetFirstPass(file);
        auto vertexShader = _GetFirstShader(pass, CompileStatement::VertexShader);
        auto pixelShader = _GetFirstShader(pass, CompileStatement::PixelShader);
        return _Compile(vertexShader.entry, pixelShader.entry, source, Profile::Glsl);
    }

    std::pair<std::string, std::string> HlslFileToHlsl(File const& file, std::string const& source)
    {
        auto pass = _GetFirstPass(file);
        auto vertexShader = _GetFirstShader(pass, CompileStatement::VertexShader);
        auto pixelShader = _GetFirstShader(pass, CompileStatement::PixelShader);
        return _Compile(vertexShader.entry, pixelShader.entry, source, Profile::Hlsl);
    }

    static Type::Type _ParseType(std::string type)
    {
        if (type == "sampler2D")
            return Type::Sampler2D;
        if (type == "bool")
            return Type::Boolean;
        if (type == "float")
            return Type::Float;
        if (type == "float2")
            return Type::Float2;
        if (type == "float3")
            return Type::Float3;
        if (type == "float3x3")
            return Type::Float3x3;
        if (type == "float4")
            return Type::Float4;
        if (type == "float4x4")
            return Type::Float4x4;
        throw std::runtime_error("Unknown type \"" + type + "\"");
    }

    Shader HlslFileToShader(File const& file, std::string const& source)
    {
        Shader shader;

        auto const& pass = _GetFirstPass(file);
        auto const& vertexShader = _GetFirstShader(pass, CompileStatement::VertexShader);
        auto const& pixelShader = _GetFirstShader(pass, CompileStatement::PixelShader);

        auto const& hlsl = _Compile(vertexShader.entry, pixelShader.entry, source, Profile::Hlsl);
        auto const& glsl = _Compile(vertexShader.entry, pixelShader.entry, source, Profile::Glsl);
        shader.hlslVertex = hlsl.first;
        shader.hlslPixel = hlsl.second;
        shader.glslVertex = glsl.first;
        shader.glslPixel = glsl.second;

        for (auto const& stmt: pass.statements)
        {
            if (stmt.which() == 0)
            {
                auto const& state = boost::get<PassStatement>(stmt);
                if (shader.deviceStates.find(state.key) != shader.deviceStates.end())
                    throw std::runtime_error("multiple value for \"" + state.key + "\"");
                shader.deviceStates.insert(std::make_pair(state.key, state.value));
            }
        }

        for (auto const& stmt: file.statements)
        {
            if (stmt.which() == 0)
            {
                auto const& var = boost::get<Variable>(stmt);
                UniformParameter param;
                param.type = _ParseType(var.type);
                if (var.value.which() == 1)
                {
                    Sampler sampler;
                    for (auto const& state: boost::get<SamplerState>(var.value).states)
                    {
                        if (sampler.states.find(state.key) != sampler.states.end())
                            throw std::runtime_error("multiple value for sampler_state near \"" + state.key + "\"");
                        sampler.states.insert(std::make_pair(state.key, state.value));
                    }
                    param.value = sampler;
                }
            }
        }

        // todo: shader.attributes


        return shader;
    }

    void SerializeShader(Shader const& shader, std::ostream& out)
    {
    }

}