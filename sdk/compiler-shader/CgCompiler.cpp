#include "precompiled.hpp"

#include "CgCompiler.hpp"

namespace Hlsl {

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

    std::string HlslFileToGlsl(File const& file, std::string const& source)
    {
        std::stringstream tmp;
        auto pass = _GetFirstPass(file);
        auto vertexShader = _GetFirstShader(pass, CompileStatement::VertexShader);
        auto pixelShader = _GetFirstShader(pass, CompileStatement::PixelShader);
        auto ctx = cgCreateContext();

        auto program = cgCreateProgram(ctx, CG_SOURCE, source.c_str(), CG_PROFILE_ARBVP1, vertexShader.entry.c_str(), 0);
        if (program == 0)
        {
            std::cout << "Error " << cgGetError() << ": " << cgGetErrorString(cgGetError()) << std::endl;
            std::cout << cgGetLastListing(ctx) << std::endl;
        }
        tmp << cgGetProgramString(program, CG_COMPILED_PROGRAM);
        program = cgCreateProgram(ctx, CG_SOURCE, source.c_str(), CG_PROFILE_ARBFP1, pixelShader.entry.c_str(), 0);
        if (program == 0)
        {
            std::cout << "Error " << cgGetError() << ": " << cgGetErrorString(cgGetError()) << std::endl;
            std::cout << cgGetLastListing(ctx) << std::endl;
        }
        tmp << cgGetProgramString(program, CG_COMPILED_PROGRAM);

        cgDestroyProgram(program);
        cgDestroyContext(ctx);

        return tmp.str();
    }

    std::string HlslFileToHlsl(File const& file, std::string const& source)
    {
        std::stringstream tmp;
        auto pass = _GetFirstPass(file);
        auto vertexShader = _GetFirstShader(pass, CompileStatement::VertexShader);
        auto pixelShader = _GetFirstShader(pass, CompileStatement::PixelShader);
        auto ctx = cgCreateContext();

        auto program = cgCreateProgram(ctx, CG_SOURCE, source.c_str(), CG_PROFILE_VS_3_0, vertexShader.entry.c_str(), 0);
        if (program == 0)
        {
            std::cout << "Error " << cgGetError() << ": " << cgGetErrorString(cgGetError()) << std::endl;
            std::cout << cgGetLastListing(ctx) << std::endl;
        }
        tmp << cgGetProgramString(program, CG_COMPILED_PROGRAM);

        cgDestroyProgram(program);
        cgDestroyContext(ctx);

        return tmp.str();
    }

}