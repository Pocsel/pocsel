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

#define STRINGIFY(...) #__VA_ARGS__

int main(int ac, char** av)
{
    using namespace Hlsl;
    std::string tmp;

    // TESTS parseur HLSL
    tmp = STRINGIFY(
        float4x4 toto : WorldViewProjection = mul(a, b);
        float4 position : POSITION;
        sampler2D) "/* test */" STRINGIFY(toto = sampler_state {};

        float4 toto(float4 bibi : POSITION, float4x4 toto : TEXCOORD1) : POSITION
        {
            toto;
            titi;
            for) " /* test */" STRINGIFY((int i = 10; i < 10; i++)
            {
                test();
            }
        }

        technique tech
        {
            pass p0
            {
                titi = toto;
                AlphaBlendEnable = false;
                VertexShader = compile vs_2_0 vs();
                PixelShader = compile ps_2_0 fs();
            }
        }
    );
    tmp = "//comment\r\n" + tmp + "// test comment\r\n/* gdfhjgkdsfhkjgh \r\n sdfgjkfsn */\r\nfloat4 testComment : COMMENT = test des commentaires en fin de fichier;//test";

    File file;
    std::stringstream ss(tmp);
    if (ParseStream(ss, file))
    {
        std::cout << "file: " << std::endl;
        for (auto& s: file.statements)
            switch (s.which()) //<Hlsl::Variable, Hlsl::Function, Hlsl::Technique, std::string>
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
                    std::cout << "{" << std::endl;
                    for (auto& statement: func.statements)
                        std::cout << "\t" << statement.statement << ";" << std::endl;
                    std::cout << "}" << std::endl;
                }
                break;
            case 2:
                {
                    auto& tech = boost::get<Technique>(s);
                    std::cout << "Technique: " << tech.name << ":\n";
                    for (auto& pass: tech.passes)
                    {
                        std::cout << "\tPass: " << pass.name << std::endl;
                        for (auto& kvp: pass.statements)
                            std::cout << "\t\t" << kvp.key << ": " << kvp.value << std::endl;
                    }
                }
                break;
            case 3:
                {
                    auto& str = boost::get<std::string>(s);
                    std::cout << "Unknown: \"" << str << "\"\n";
                }
                break;
            }
    }

    std::cin.get();
    return 0;
}
