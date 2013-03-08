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

int main(int ac, char** av)
{
    using namespace Hlsl;

    if (ac <= 2)
    {
        std::cerr << "Usage: compiler-shader <shader.fx> <output.fxc>" << std::endl;
        return 1;
    }

    // Macros - base
    std::list<std::string> macros;
    macros.push_front("POCSEL");

    // OpenGL only
    auto macrosGL = macros;
    macrosGL.push_front("OPENGL");

    // DirectX only
    auto macrosDX = macros;
    macrosDX.push_front("DIRECTX");

    File fileGL;
    File fileDX;

    {
        std::ifstream in(av[1]);
        if (!ParseStream(in, fileGL, macrosGL))
        {
            std::cout << "Error when parsing OpenGL version\n";
            return 1;
        }
    }
    {
        std::ifstream in(av[1]);
        if (!ParseStream(in, fileDX, macrosDX))
        {
            std::cout << "Error when parsing DirectX 9 version\n";
            return 1;
        }
    }

    GeneratorOptions options;
    options.removeSemanticAttributes = true;

    auto const& srcGL = GenerateHlsl(fileGL, options);
    options.removeSemanticAttributes = false;
    auto const& srcDX = GenerateHlsl(fileDX, options);
    auto const& shader = HlslFileToShader(fileGL, srcGL, fileDX, srcDX);

    std::ofstream out(av[2], std::ios::binary | std::ios::out);
    SerializeShader(shader, out);
#ifdef DEBUG
    std::cout << "Success\n";
#endif

#if defined(_WINDOWS) && defined(DEBUG)
    std::cin.get();
#endif
    return 0;
}
