#include "precompiled.hpp"

#include "sdk/compiler-shader/HlslParser.hpp"

template<class TOut>
void printVariable(TOut& out, Hlsl::Variable const& var)
{
    out << "Variable: " << var.type << " " << var.name << " : " << var.semantic << " = ";
    switch (var.value.which())
    {
        case 0: out << boost::get<Hlsl::Statement>(var.value).statement << std::endl; break;
        case 1:
            {
                out << "sampler_state {" << std::endl;
                for (auto& ds: boost::get<Hlsl::SamplerState>(var.value).states)
                    out << "\t\t" << ds.key << " = " << ds.value << std::endl;
                out << "\t}" << std::endl;
            }
            break;
    }
}

template<class TOut>
void printCodeBlock(TOut& out, Hlsl::CodeBlock const& codeBlock, std::string const& tab = "")
{
    out << tab << "{" << std::endl;
    for (auto const& statement: codeBlock.statements)
        switch (statement.which())
        {
            case 0: out << tab << "\t" << boost::get<Hlsl::Statement>(statement).statement << std::endl; break;
            case 1: printCodeBlock(out, boost::get<Hlsl::CodeBlock>(statement), tab + "\t"); break;
        }
    out << tab << "}" << std::endl;
}

template<class TOut>
void printPassStatement(TOut& out, Hlsl::Pass const& pass, std::string const& tab = "")
{
    out << tab << "Pass: " << pass.name << std::endl;
    for (auto const& v: pass.statements)
        switch (v.which())
        {
        case 0:
            {
                auto const& kvp = boost::get<Hlsl::PassStatement>(v);
                out << tab << "\t" << kvp.key << ": " << kvp.value << std::endl;
            }
            break;
        case 1:
            {
                auto const& c = boost::get<Hlsl::CompileStatement>(v);
                out << tab << "\t" << (c.type == Hlsl::CompileStatement::PixelShader ? "PixelShader" : "VertexShader") << ": entry \"" << c.entry << "\" and profile \"" << c.profile << "\".\n";
            }
            break;
        }
}

#define STRINGIFY(...) #__VA_ARGS__

int main(int ac, char** av)
{
    using namespace Hlsl;
    std::string tmp;

    // TESTS parseur HLSL
    tmp = STRINGIFY(
        float4x4 mvp : WorldViewProjection = mul(a, b);
        float4 position : POSITION;
        sampler2D) "/* test */" STRINGIFY(toto = sampler_state {};
        int a;

        float4 simple()
        {
            return float4(0, 0, 0, 0);
        }

        float4 dfsgjkh(float4 bibi : POSITION) : POSITION
        {
            simple();
            simple();
            for) " /* test\n */\t" STRINGIFY((int i = 10; i < 10; i++)
            {
                simple();
                for (int i = 1; i > 10; i++)
                    simple();
                for (int i = 1; i > 10; i++)
                {
                    simple();
                }
            }
            do {
            }while(x);
            return simple();
        }

        float4 vs(float4 pos : POSITION) : POSITION
        {
            return mul(mvp, pos);
        }

        float4 fs() : COLOR
        {
            return simple();
        }

        technique tech
        {
            pass p0
            {
                AlphaBlendEnable = false;
                VertexShader = compile vs_2_0 vs();
                PixelShader = compile ps_2_0 fs();
            }
        }
    );
    //tmp = "//comment\r\n" + tmp + "// test comment\r\n/* gdfhjgkdsfhkjgh \r\n sdfgjkfsn */\r\nfloat4 testComment : COMMENT = test des commentaires en fin de fichier;;;//test\n";

    File file;
    std::stringstream ss(tmp);
    if (ParseStream(ss, file))
    {
        std::cout << "file: " << std::endl;
        for (auto& s: file.statements)
            switch (s.which()) //<Hlsl::Variable, Hlsl::Function, Hlsl::Technique, Hlsl::Nil>
            {
            case 0:
                printVariable(std::cout, boost::get<Variable>(s));
                break;
            case 1:
                {
                    auto& func = boost::get<Function>(s);
                    std::cout << "Function: " << func.ret << " " << func.name << "(" << std::endl;
                    for (auto& var: func.arguments)
                        printVariable(std::cout << "\t", var);
                    std::cout << ") : " << func.semantic << std::endl;
                    printCodeBlock(std::cout, func.code);
                }
                break;
            case 2:
                {
                    auto& tech = boost::get<Technique>(s);
                    std::cout << "Technique: " << tech.name << ":\n";
                    for (auto& pass: tech.passes)
                        printPassStatement(std::cout, pass, "\t");
                }
                break;
            case 3:
                std::cout << "Empty\n";
                break;
            }
    }


    std::cout << "\n\n------------ CG -----------\n";

    auto getFirstPass = [](File const& file) -> Pass const&
        {
            for (auto const& s: file.statements)
                if (s.which() == 2)
                    for (auto const& pass: boost::get<Technique>(s).passes)
                        return pass;
            throw std::runtime_error("can't find any pass");
        };
    auto getFirstShader = [](Pass const& pass, CompileStatement::ShaderType type) -> CompileStatement
        {
            for (auto const& s: pass.statements)
                if (s.which() == 1 && boost::get<CompileStatement>(s).type == type)
                    return boost::get<CompileStatement>(s);
            throw std::runtime_error(std::string("can't find ") + (type == CompileStatement::PixelShader ? "PixelShader" : "VertexShader") + " in pass " + pass.name);
        };


    auto pass = getFirstPass(file);
    auto vertexShader = getFirstShader(pass, CompileStatement::VertexShader);
    auto pixelShader = getFirstShader(pass, CompileStatement::PixelShader);
    auto ctx = cgCreateContext();
    auto program = cgCreateProgram(ctx, CG_SOURCE, tmp.c_str(), CG_PROFILE_HLSLV, vertexShader.entry.c_str(), 0);
    if (program == 0)
    {
        std::cout << "Error " << cgGetError() << ": " << cgGetErrorString(cgGetError()) << std::endl;
        std::cout << cgGetLastListing(ctx) << std::endl;
    }

    std::cout << cgGetProgramString(program, CG_COMPILED_PROGRAM) << std::endl;

    std::cin.get();
    return 0;
}
