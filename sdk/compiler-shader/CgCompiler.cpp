#include "precompiled.hpp"

#include "sdk/compiler-shader/CgCompiler.hpp"
#include "sdk/compiler-shader/HlslGenerator.hpp"

namespace {
    struct _TupleStringStringInt
    {
        std::string identifier;
        std::string generatedIdentifier;
        int index;
    };
}
BOOST_FUSION_ADAPT_STRUCT(
    _TupleStringStringInt,
    (std::string, identifier)
    (std::string, generatedIdentifier)
    (int, index)
)


namespace Hlsl {

    namespace {
        static CGprofile _GetCGProfile(Profile::Type profile, CompileStatement::ShaderType type)
        {
            switch (profile)
            {
            case Profile::Hlsl: return type == CompileStatement::VertexShader ? CG_PROFILE_HLSLV : CG_PROFILE_HLSLF;
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

        static Function const& _GetFunction(File const& file, std::string const& functionName)
        {
            for (auto const& s: file.statements)
                if (s.which() == 1)
                {
                    auto const& func = boost::get<Function>(s);
                    if (func.name == functionName)
                        return func;
                }
            throw std::runtime_error("can't find function \"" + functionName + "\"");
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

        static std::string _GetValue(boost::variant<bool, Sampler, std::array<float, 16>, Nil> const& value)
        {
            struct : public boost::static_visitor<std::string>
            {
                std::string operator()(bool v) { return v ? "true" : "false"; }
                std::string operator()(Sampler const& sampler)
                {
                    std::stringstream ss;
                    ss << "{";
                    for (auto const& s: sampler.states)
                        ss << s.first << "=" << s.second << ";";
                    ss << "}";
                    return ss.str();
                }
                std::string operator()(std::array<float, 16> const& arr)
                {
                    std::stringstream ss;
                    ss << "(";
                    for (auto const& v: arr)
                        ss << v << ";";
                    ss << ")";
                    return ss.str();
                }
                std::string operator()(Nil const&)
                {
                    return "-";
                }
            } visitor;
            return boost::apply_visitor(visitor, value);
        }
    }

    // Parsing
    namespace {
        namespace ascii = boost::spirit::ascii;
        namespace qi = boost::spirit::qi;
        using qi::char_;
        using qi::lit;
        using ascii::string;
        using namespace qi::labels;
        using boost::phoenix::val;
        using boost::phoenix::construct;
        using boost::phoenix::at_c;
        using boost::phoenix::push_back;

        static std::list<std::pair<Variable const*, std::string>> _ParseGeneratedXlsl(std::string const& xlsl, File const& file, Function const& func)
        {
            auto it = xlsl.begin();
            auto ite = xlsl.end();

            qi::rule<decltype(it), std::string()> identifierParser;
            identifierParser %= char_("a-zA-Z_") >> *char_("a-zA-Z0-9_.");
            qi::rule<decltype(it), _TupleStringStringInt(), ascii::space_type> parameterParser;
            qi::rule<decltype(it), std::list<_TupleStringStringInt>(), ascii::space_type> fileParser;

            parameterParser = 
                    "//var" >> identifierParser >> identifierParser[at_c<0>(_val) = _1] >> ':' // type identifier
                    >> *(char_ - ':') >> ':' // semantic
                    >> identifierParser[at_c<1>(_val) = _1] >> -('[' >> *char_("0-9-") >> ']') >> -(',' >> *char_("0-9-")) >> ':' // generated name
                    >> qi::int_[at_c<2>(_val) = _1] >> ':' // index
                    >> *char_("0-9-") // ?
                ;

            fileParser =
                    *(
                        parameterParser[push_back(_val, _1)]
                        |
                        (-lit("//") >> +(char_ - '/'))
                    )
                    >> qi::eoi
                ;

            std::list<_TupleStringStringInt> tmp;
            qi::phrase_parse(it, ite, fileParser, ascii::space, tmp);

            std::list<std::pair<Variable const*, std::string>> result;
            for (auto& var: tmp)
            {
                if (var.identifier.find('.') != std::string::npos)
                    continue;
                std::function<Variable const*()> findVar;
                if (var.index >= 0)
                    findVar = [&]() -> Variable const* {
                            for (auto const& arg: func.arguments)
                                if (arg.in && arg.name == var.identifier)
                                    return &arg;
                            return nullptr;
                        };
                else
                    findVar = [&]() -> Variable const* {
                            for (auto const& stmt: file.statements)
                                if (stmt.which() == 0)
                                {
                                    auto const& uniform = boost::get<Variable>(stmt);
                                    if (uniform.name == var.identifier)
                                        return &uniform;
                                }
                            return nullptr;
                        };
                if (var.generatedIdentifier != "")
                    result.push_back(std::make_pair(findVar(), var.generatedIdentifier));
            }
            result.sort();
            return result;
        }

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

    Shader HlslFileToShader(File const& file, std::string const& source)
    {
        Shader shader;
        shader.source = source;

        auto const& pass = _GetFirstPass(file);
        auto const& vertexShader = _GetFirstShader(pass, CompileStatement::VertexShader);
        auto const& pixelShader = _GetFirstShader(pass, CompileStatement::PixelShader);
        auto const& functionVertex = _GetFunction(file, vertexShader.entry);
        auto const& functionPixel = _GetFunction(file, pixelShader.entry);

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

        auto tmpGlslv = _ParseGeneratedXlsl(shader.glslVertex, file, functionVertex);
        auto tmpHlslv = _ParseGeneratedXlsl(shader.hlslVertex, file, functionVertex);
        tmpGlslv.merge(_ParseGeneratedXlsl(shader.glslPixel, file, functionPixel));
        tmpHlslv.merge(_ParseGeneratedXlsl(shader.hlslPixel, file, functionPixel));

        // shader.uniforms
        for (auto const& stmt: file.statements)
        {
            if (stmt.which() == 0)
            {
                auto const& var = boost::get<Variable>(stmt);
                UniformParameter param;
                for (auto const& v: tmpGlslv)
                    if (v.first == &var)
                        param.openGL = v.second;
                for (auto const& v: tmpHlslv)
                    if (v.first == &var)
                        param.directX = v.second;
                param.type = _ParseType(var.type);
                param.value = Nil();
                switch (var.value.which())
                {
                case 0:
                    {
                        std::string stat = boost::get<Statement>(var.value).statement;
                        if (stat == "")
                            break;
                        switch (param.type)
                        {
                        case Type::Boolean:
                            {
                                if (stat == "true")
                                    param.value = true;
                                else if (stat == "false")
                                    param.value = false;
                                else
                                    throw std::runtime_error("the default value must be a constant (check \"" + var.name + "\")");
                            }
                            break;
                        case Type::Float:
                            {
                                std::stringstream ss(stat);
                                std::array<float, 16> f;
                                ss >> f[0];
                                param.value = f;
                            }
                            break;
                        }
                    }
                    break;
                case 1:
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
                    break;
                }
                shader.uniforms[var.name] = param;
            }
        }

        // shader.attributes
        for (auto const& arg: functionVertex.arguments)
        {
            for (auto const& v: tmpGlslv)
                if (v.first == &arg)
                    shader.attributes[v.first->name].openGL = v.second;
            for (auto const& v: tmpHlslv)
                if (v.first == &arg)
                    shader.attributes[v.first->name].directX = v.second;
        }

        return shader;
    }

    void SerializeShader(Shader const& shader, std::ostream& out)
    {
        out << "source " << shader.source.size() << "\n" << shader.source;
        out << "glslv " << shader.glslVertex.size() << "\n" << shader.glslVertex;
        out << "glslf " << shader.glslPixel.size() << "\n" << shader.glslPixel;
        out << "hlslv " << shader.hlslVertex.size() << "\n" << shader.hlslVertex;
        out << "hlslf " << shader.hlslPixel.size() << "\n" << shader.hlslPixel;
        out << "attributes " << shader.attributes.size() << "\n";
        for (auto const& attr: shader.attributes)
            out << attr.first << " " << attr.second.openGL << " " << attr.second.directX << "\n";
        out << "uniforms " << shader.uniforms.size() << "\n";
        for (auto const& uniform: shader.uniforms)
            out << uniform.first << " " << uniform.second.openGL << " " << uniform.second.directX << " " << _GetValue(uniform.second.value) << "\n";
        out << "states " << shader.deviceStates.size() << "\n";
        for (auto const& state: shader.deviceStates)
            out << state.first << " " << state.second << "\n";
    }

}