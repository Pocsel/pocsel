#include "precompiled.hpp"

#include "sdk/compiler-shader/CgCompiler.hpp"
#include "sdk/compiler-shader/HlslGenerator.hpp"

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

template<class TOut>
void printFile(TOut& out, Hlsl::File const& file)
{
    using namespace Hlsl;
    for (auto& s: file.statements)
        switch (s.which()) //<Hlsl::Variable, Hlsl::Function, Hlsl::Technique, Hlsl::Structure, Hlsl::Nil>
        {
        case 0:
            printVariable(out, boost::get<Variable>(s));
            break;
        case 1:
            {
                auto& func = boost::get<Function>(s);
                out << "Function: " << func.ret << " " << func.name << "(" << std::endl;
                for (auto& var: func.arguments)
                    printVariable(out << "\t", var);
                out << ") : " << func.semantic << std::endl;
                printCodeBlock(out, func.code);
            }
            break;
        case 2:
            {
                auto& tech = boost::get<Technique>(s);
                out << "Technique: " << tech.name << ":\n";
                for (auto& pass: tech.passes)
                    printPassStatement(out, pass, "\t");
            }
            break;
        case 3:
            {
                auto& struc = boost::get<Structure>(s);
                out << "Structure: " << struc.name << ":\n";
                for (auto& var: struc.members)
                    printVariable(out << "\t", var);
            }
            break;
        case 4:
            out << "Empty\n";
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
        float4x4 model : World;
        float4x4 view : View;
        float4x4 projection : Projection;
        float4x4 mvp : WorldViewProjection = mul(a, b);
        float4 position : POSITION;
        sampler2D) "/* test */" STRINGIFY(toto = sampler_state {     MinFilter = LinearMipMapLinear;
    MagFilter = Nearest; };
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

        struct vsOut
        {
            float4 pos : POSITION;
            float2 tex : TEXCOORD0;
        };

        vsOut vs(float4 pos : POSITION, float2 tex : TEXCOORD0, float4 testVar) : POSITION
        {
            vsOut o;
            o.pos = mul(model * view * projection, pos);
            o.tex = tex * testVar.xy;
            return o;
        }

        float4 fs(vsOut i) : COLOR
        {
            i.tex = mul((float2x2)mvp, i.tex);
            return tex2D(toto, i.tex);
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
        //printFile(std::cout, file);
        std::cout << GenerateHlsl(file);
    }
    else
        std::cout << "Parsing error" << std::endl;


    std::cout << "\n\n------------ CG -----------\n *** OpenGL\n";

    GeneratorOptions options;
    options.removeSemanticAttributes = true;
    tmp = GenerateHlsl(file, options);

    auto pair = HlslFileToGlsl(file, tmp);
    std::cout << pair.first << pair.second << std::endl << " *** DIRECTX\n";
    pair = HlslFileToHlsl(file, tmp);
    std::cout << pair.first << pair.second << std::endl;

#ifdef _WINDOWS
    std::cin.get();
#endif
    return 0;
}
