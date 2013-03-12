#include "precompiled.hpp"
#include <boost/algorithm/string.hpp>

#include "tools/ByteArray.hpp"

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
            //case Profile::Hlsl: return type == CompileStatement::VertexShader ? CG_PROFILE_VS_3_0 : CG_PROFILE_PS_3_0;
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

        static Type::Type _ParseType(std::string const& type)
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

        static Semantic::Type _ParseSemantic(std::string const& str)
        {
            auto const& semantic = boost::to_lower_copy(str);
            // *** Uniforms
            // Matrices
            if (semantic == "world")
                return Semantic::World;
            if (semantic == "view")
                return Semantic::View;
            if (semantic == "projection")
                return Semantic::Projection;
            if (semantic == "worldview")
                return Semantic::WorldView;
            if (semantic == "viewprojection")
                return Semantic::ViewProjection;
            if (semantic == "worldviewprojection")
                return Semantic::WorldViewProjection;
            // Inverse
            if (semantic == "worldinverse")
                return Semantic::WorldInverse;
            if (semantic == "viewinverse")
                return Semantic::ViewInverse;
            if (semantic == "projectioninverse")
                return Semantic::ProjectionInverse;
            if (semantic == "worldviewinverse")
                return Semantic::WorldViewInverse;
            if (semantic == "viewprojectioninverse")
                return Semantic::ViewProjectionInverse;
            if (semantic == "worldviewprojectioninverse")
                return Semantic::WorldViewProjectionInverse;
            // Inverse transpose
            if (semantic == "worldinversetranspose")
                return Semantic::WorldInverseTranspose;
            if (semantic == "viewinversetranspose")
                return Semantic::ViewInverseTranspose;
            if (semantic == "projectioninversetranspose")
                return Semantic::ProjectionInverseTranspose;
            if (semantic == "worldviewinversetranspose")
                return Semantic::WorldViewInverseTranspose;
            if (semantic == "viewprojectioninversetranspose")
                return Semantic::ViewProjectionInverseTranspose;
            if (semantic == "worldviewprojectioninversetranspose")
                return Semantic::WorldViewProjectionInverseTranspose;

            // *** Attributes
            if (semantic == "position")
                return Semantic::Position;
            if (semantic == "normal")
                return Semantic::Normal;
            if (semantic == "color")
                return Semantic::Color;
            if (semantic == "texcoord0")
                return Semantic::TexCoord0;
            if (semantic == "texcoord1")
                return Semantic::TexCoord1;
            if (semantic == "texcoord2")
                return Semantic::TexCoord2;
            if (semantic == "texcoord3")
                return Semantic::TexCoord3;
            if (semantic == "texcoord4")
                return Semantic::TexCoord4;
            if (semantic == "")
                return Semantic::NoSemantic;
            throw std::runtime_error("Unknown semantic \"" + semantic + "\"");
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
                Variable const* findedVar = nullptr;
                if (var.index >= 0)
                    findedVar = [&]() -> Variable const* {
                            for (auto const& arg: func.arguments)
                                if (arg.in && arg.name == var.identifier)
                                    return &arg;
                            return nullptr;
                        }();
                else
                    findedVar = [&]() -> Variable const* {
                            for (auto const& stmt: file.statements)
                                if (stmt.which() == 0)
                                {
                                    auto const& uniform = boost::get<Variable>(stmt);
                                    if (uniform.name == var.identifier)
                                        return &uniform;
                                }
                            return nullptr;
                        }();
                if (findedVar != nullptr && var.generatedIdentifier != "")
                    result.push_back(std::make_pair(findedVar, var.generatedIdentifier));
            }
            result.sort();
            for (auto& pair: result)
                std::cout << pair.first->name << " => " << pair.second << std::endl;
            return result;
        }

    }

    Shader HlslFileToShader(File const& file, std::string const& source, Profile::Type profile)
    {
        Shader shader;
        shader.source = source;

        auto const& pass = _GetFirstPass(file);
        auto const& vertexShader = _GetFirstShader(pass, CompileStatement::VertexShader);
        auto const& pixelShader = _GetFirstShader(pass, CompileStatement::PixelShader);
        auto const& functionVertex = _GetFunction(file, vertexShader.entry);
        auto const& functionPixel = _GetFunction(file, pixelShader.entry);

        auto const& tmpCompiled = _Compile(vertexShader.entry, pixelShader.entry, source, profile);
        shader.vertex = tmpCompiled.first;
        shader.pixel = tmpCompiled.second;

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

        auto params = _ParseGeneratedXlsl(shader.vertex, file, functionVertex);
        params.merge(_ParseGeneratedXlsl(shader.pixel, file, functionPixel));

        // shader.uniforms
        for (auto const& stmt: file.statements)
        {
            if (stmt.which() == 0)
            {
                auto const& var = boost::get<Variable>(stmt);
                UniformParameter param;
                bool finded = false;
                for (auto const& v: params)
                {
                    if (v.first == &var)
                    {
                        param.name = v.second;
                        finded = true;
                    }
                }
                if (!finded)
                    continue;
                param.semantic = _ParseSemantic(var.semantic);
                if ((Semantic::UniformFirst > param.semantic || param.semantic > Semantic::UniformLast) && param.semantic != Semantic::NoSemantic)
                    throw std::runtime_error("Bad semantic for uniform variable \"" + var.name + "\"");
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
            for (auto const& v: params)
                if (v.first == &arg)
                {
                    auto semantic = _ParseSemantic(v.first->semantic);
                    if (Semantic::AttributeFirst > semantic || semantic > Semantic::AttributeLast)
                        throw std::runtime_error("Bad semantic for attribute variable \"" + v.first->name + "\"");
                    auto& att = shader.attributes[v.first->name];
                    att.semantic = semantic;
                    att.name = v.second;
                }

        return shader;
    }

    CompleteShader HlslFileToShader(File const& fileGL, std::string const& sourceGL, File const& fileDX, std::string const& sourceDX)
    {
        CompleteShader shader;
        shader.glsl = HlslFileToShader(fileGL, sourceGL, Profile::Glsl);
        shader.hlsl = HlslFileToShader(fileDX, sourceDX, Profile::Hlsl);
        return shader;
    }

    void SerializeShader(Shader const& shader, Tools::ByteArray& bin)
    {
        // Shaders
        auto writeLongString = [&](std::string const& str) { bin.Write32((Uint32)str.size()); bin.WriteRawData(str.data(), (Uint32)str.size()); };
        writeLongString(shader.source);
        writeLongString(shader.vertex);
        writeLongString(shader.pixel);

        // Attributes
        bin.Write32((Uint32)shader.attributes.size());
        for (auto const& attr: shader.attributes)
        {
            bin.Write16((Uint16)attr.second.semantic);
            bin.WriteString(attr.first); // original name
            bin.WriteString(attr.second.name);
        }

        // Uniforms
        bin.Write32((Uint32)shader.uniforms.size());
        for (auto const& uniform: shader.uniforms)
        {
            bin.Write16((Uint16)uniform.second.semantic);
            bin.WriteString(uniform.first); // original name
            bin.Write8((Uint8)uniform.second.type);
            bin.WriteString(uniform.second.name);
            bin.WriteString(_GetValue(uniform.second.value)); // Default value
        }

        // States
        bin.Write32((Uint32)shader.deviceStates.size());
        for (auto const& state: shader.deviceStates)
        {
            bin.WriteString(state.first); // state name
            bin.WriteString(state.second); // value
        }
    }

    void SerializeShader(CompleteShader const& shader, std::ostream& out)
    {
        Tools::ByteArray bin;
        bin.Write32(0xB16B00B5); // BIGBOOBS magic code

        SerializeShader(shader.glsl, bin);
        SerializeShader(shader.hlsl, bin);

        out.write(bin.GetData(), bin.GetSize());
    }

}