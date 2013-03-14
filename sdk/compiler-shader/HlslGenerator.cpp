#include "precompiled.hpp"

#include "sdk/compiler-shader/HlslGenerator.hpp"

namespace Hlsl {

    template<class TOut>
    inline void _PrintVariable(TOut& out, Hlsl::Variable const& var, bool showSemantic = true)
    {
        out << var.type << " " << var.name;
        if (var.arraySize.length() > 0)
            out << '[' << var.arraySize << ']';
        if (showSemantic && var.semantic.length() > 0)
            out << ": " << var.semantic;
        switch (var.value.which())
        {
            case 0:
                {
                    auto& stmt = boost::get<Hlsl::Statement>(var.value);
                    if (stmt.statement.length() > 0)
                        out << " = " << stmt.statement << std::endl;
                }
                break;
            case 1:
                {
                    out << " = sampler_state {\n";
                    for (auto& ds: boost::get<Hlsl::SamplerState>(var.value).states)
                        out << "\t\t" << ds.key << " = " << ds.value << ";\n";
                    out << "}\n";
                }
                break;
        }
    }

    template<class TOut>
    inline void _PrintCodeBlock(TOut& out, Hlsl::CodeBlock const& codeBlock, std::string const& tab = "")
    {
        out << tab << "{\n";
        for (auto const& statement: codeBlock.statements)
            switch (statement.which())
            {
                case 0: out << tab + "\t" << boost::get<Hlsl::Statement>(statement).statement << "\n"; break;
                case 1: _PrintCodeBlock(out, boost::get<Hlsl::CodeBlock>(statement), tab + "\t"); break;
            }
        out << tab << "}\n";
    }

    template<class TOut>
    inline void _PrintPassStatement(TOut& out, Hlsl::Pass const& pass, std::string const& tab = "")
    {
        out << tab << "pass " << pass.name << '\n' << tab << "{\n";
        for (auto const& v: pass.statements)
            switch (v.which())
            {
            case 0:
                {
                    auto const& kvp = boost::get<Hlsl::PassStatement>(v);
                    out << tab << "\t" << kvp.key << " = " << kvp.value << ";\n";
                }
                break;
            case 1:
                {
                    auto const& c = boost::get<Hlsl::CompileStatement>(v);
                    out << tab << "\t" << (c.type == Hlsl::CompileStatement::PixelShader ? "PixelShader" : "VertexShader") << " = compile " << c.profile << " " << c.entry << "();\n";
                }
                break;
            }
        out << tab << "}\n";
    }

    template<class TOut>
    inline void _PrintFile(TOut& out, Hlsl::File const& file, GeneratorOptions const& options)
    {
        using namespace Hlsl;
        for (auto& s: file.statements)
            switch (s.which()) //<Hlsl::Variable, Hlsl::Function, Hlsl::Technique, Hlsl::Structure, Hlsl::Nil>
            {
            case 0:
                _PrintVariable(out, boost::get<Variable>(s));
                out << ";\n";
                break;
            case 1:
                {
                    auto& func = boost::get<Function>(s);
                    out << func.ret << " " << func.name << "(";
                    bool first = true;
                    for (auto& var: func.arguments)
                    {
                        if (first)
                            first = false;
                        else
                            out << ", ";
                        if (var.in && var.out)
                            out << "inout ";
                        else if (var.in)
                            out << "in ";
                        else if (var.out)
                            out << "out ";
                        _PrintVariable(out, var, (options.removeSemanticAttributes && var.out) || !options.removeSemanticAttributes);
                    }
                    out << ")";
                    if (func.semantic.length() > 0)
                         out << ": " << func.semantic;
                    out << "\n";
                    _PrintCodeBlock(out, func.code);
                }
                break;
            case 2:
                {
                    auto& tech = boost::get<Technique>(s);
                    out << "technique " << tech.name << "\n{\n";
                    for (auto& pass: tech.passes)
                        _PrintPassStatement(out, pass, "\t");
                    out << "}\n";
                }
                break;
            case 3:
                {
                    auto& struc = boost::get<Structure>(s);
                    out << "struct " << struc.name << "\n{\n";
                    for (auto& var: struc.members)
                    {
                        out << "\t";
                        _PrintVariable(out, var);
                        out << ";\n";
                    }
                    out << "};\n";
                }
                break;
            case 4:
                //out << "Empty\n";
                break;
            }
    }

    std::string GenerateHlsl(File const& file, GeneratorOptions const& options)
    {
        std::stringstream stream;
        _PrintFile(stream, file, options);
        return stream.str();
    }
}