#include "precompiled.hpp"


bool parse_numbers(char const* str, std::vector<std::string>& v)
{
    namespace qi = boost::spirit::qi;

    std::string s(str);
    std::string tmp;
    auto alnum = *qi::alpha >> +qi::digit;
    bool r;
    if (r = qi::parse(s.begin(), s.end(), alnum, tmp))
        v.push_back(tmp);
    return r;
}

template<class T>
bool parse_complex(T first, T last, std::complex<double>& c)
{
    using boost::phoenix::ref;
    using boost::spirit::qi::double_;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::phrase_parse;

    double rN = 0.0;
    double iN = 0.0;
    bool r = phrase_parse(first, last,
        (
            '(' >> double_[ref(rN) = _1]
                >> -(',' >> double_[ref(iN) = _1]) >> ')'
            | double_[ref(rN) = _1]
        ),
        boost::spirit::ascii::space);
    if (!r)
        return r;
    c.imag(iN);
    c.real(rN);
    return r;
}

namespace Hlsl
{
    // mul(worldMatrix, float4(normal, 0.0))
    struct Statement
    {
        std::string statement;
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

    // void vertexShader(float4 position : POSITION, float2 texCoord : TEXCOORD) { ... }
    struct Function
    {
        std::string ret;
        std::string name;
        std::list<Variable> arguments;
        std::string semantic;
        std::list<Statement> statements;
    };

    // VertexShader = vertexShader;
    // AlphaBlendEnable = false;
    struct PassStatement
    {
        std::string key;
        std::string value;
    };

    // pass Toto { VertexShader = vertexShader; PixelShader = pixelShader; }
    struct Pass
    {
        std::string name;
        std::list<PassStatement> statements;
    };

    // technique Toto { pass { VertexShader = vertexShader; PixelShader = pixelShader; }}
    struct Technique
    {
        std::string name;
        std::list<Pass> passes;
    };

    typedef boost::variant<Hlsl::Variable, Hlsl::Function, Hlsl::Technique> GlobalStatement;

    struct File
    {
        std::list<GlobalStatement> statements;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Statement,
    (std::string, statement)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::DeviceState,
    (std::string, key)
    (std::string, value)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::SamplerState,
    (std::list<Hlsl::DeviceState>, states)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Variable,
    (std::string, type)
    (std::string, name)
    (std::string, semantic)
    (Hlsl::StatementOrSamplerState, value)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Function,
    (std::string, ret)
    (std::string, name)
    (std::list<Hlsl::Variable>, arguments)
    (std::string, semantic)
    (std::list<Hlsl::Statement>, statements)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::PassStatement,
    (std::string, key)
    (std::string, value)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Pass,
    (std::string, name)
    (std::list<Hlsl::PassStatement>, statements)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Technique,
    (std::string, name)
    (std::list<Hlsl::Pass>, passes)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::File,
    (std::list<Hlsl::GlobalStatement>, statements)
)

namespace Hlsl {

    namespace ascii = boost::spirit::ascii;
    namespace qi = boost::spirit::qi;
    using qi::char_;
    using qi::lit;
    using ascii::string;
    using namespace qi::labels;
    using boost::phoenix::val;
    using boost::phoenix::at_c;
    using boost::phoenix::push_back;

    template<class T>
    struct IdentifierParser
    {
        IdentifierParser()
        {
            parser %= char_("_A-Za-z") >> *char_("_A-Za-z0-9");
        }

        boost::spirit::qi::rule<T, std::string()> parser;
    };

    template<class T>
    struct DeviceStateParser : boost::spirit::qi::grammar<T, DeviceState(), ascii::space_type>
    {
        DeviceStateParser() : DeviceStateParser::base_type(start)
        {
            start %= identifierParser.parser >> "=" >> identifierParser.parser;
        }

        IdentifierParser<T> identifierParser;
        qi::rule<T, DeviceState(), ascii::space_type> start;
    };

    template<class T>
    struct PassStatementParser : boost::spirit::qi::grammar<T, PassStatement(), ascii::space_type>
    {
        PassStatementParser() : PassStatementParser::base_type(start)
        {
            start %= identifierParser.parser >> "=" >> *(char_ - ';') >> ';';
        }

        IdentifierParser<T> identifierParser;
        qi::rule<T, PassStatement(), ascii::space_type> start;
    };

    template<class T>
    struct StatementParser : boost::spirit::qi::grammar<T, Statement(), ascii::space_type>
    {
        StatementParser() : StatementParser::base_type(start)
        {
            parser %= *(char_ - '{' - '}');

            start = parser[at_c<0>(_val) += _1] >> -(char_('{')[at_c<0>(_val) += _1] >> parser[at_c<0>(_val) += _1] >> char_('}')[at_c<0>(_val) += _1]);
        }

        qi::rule<T, std::string()> parser;
        qi::rule<T, Statement(), ascii::space_type> start;
    };

    template<class T>
    struct VariableParser : boost::spirit::qi::grammar<T, Variable(), ascii::space_type>
    {
        VariableParser() : VariableParser::base_type(start)
        {
            semanticParser %= (':' >> identifierParser.parser) | qi::eps[_val = val("")];

            samplerStateParser %= lit("sampler_state") >> "{" >> *(deviceStateParser >> ';') >> "}";

            valueParser = -('=' >> (samplerStateParser[_val = _1] | statementParser[_val = _1])) | qi::eps[_val = val(Statement())];

            start %=
                    identifierParser.parser // type
                    >> identifierParser.parser // name
                    >> semanticParser // semantic
                    >> valueParser // value
                ;
        }

        IdentifierParser<T> identifierParser;
        DeviceStateParser<T> deviceStateParser;
        StatementParser<T> statementParser;
        boost::spirit::qi::rule<T, std::string(), ascii::space_type> semanticParser;
        boost::spirit::qi::rule<T, SamplerState(), ascii::space_type> samplerStateParser;
        boost::spirit::qi::rule<T, StatementOrSamplerState(), ascii::space_type> valueParser;
        boost::spirit::qi::rule<T, Variable(), ascii::space_type> start;
    };

    template<class T>
    struct FunctionParser : boost::spirit::qi::grammar<T, Function(), ascii::space_type>
    {
        FunctionParser() : FunctionParser::base_type(start)
        {
            semanticParser %= (':' >> identifierParser.parser) | qi::eps[_val = val("")];

            argumentParser = variableParser[push_back(_val, _1)] % ',';
            statementsParser = (statementParser[push_back(_val, _1)] % ';') >> *lit(';');

            start %=
                    identifierParser.parser // type
                    >> identifierParser.parser // name
                    >> '(' >> argumentParser >> ')'
                    >> semanticParser // semantic
                    >> '{'
                    >> statementsParser
                    >> '}'
                ;
        }

        IdentifierParser<T> identifierParser;
        StatementParser<T> statementParser;
        VariableParser<T> variableParser;
        boost::spirit::qi::rule<T, std::list<Variable>(), ascii::space_type> argumentParser;
        boost::spirit::qi::rule<T, std::list<Statement>(), ascii::space_type> statementsParser;
        boost::spirit::qi::rule<T, std::string(), ascii::space_type> semanticParser;
        boost::spirit::qi::rule<T, Function(), ascii::space_type> start;
    };

    template<class T>
    struct TechniqueParser : boost::spirit::qi::grammar<T, Technique(), ascii::space_type>
    {
        TechniqueParser() : TechniqueParser::base_type(start)
        {
            passParser =
                    "pass"
                    >> identifierParser.parser[at_c<0>(_val) = _1] // name
                    >> "{"
                    >> *(passStatementParser[push_back(at_c<1>(_val), _1)])
                    >> "}"
                ;

            start =
                    "technique"
                    >> identifierParser.parser[at_c<0>(_val) = _1] // name
                    >> "{"
                    >> *(passParser[push_back(at_c<1>(_val), _1)])
                    >> "}"
                ;
        }

        IdentifierParser<T> identifierParser;
        PassStatementParser<T> passStatementParser;
        boost::spirit::qi::rule<T, Pass(), ascii::space_type> passParser;
        boost::spirit::qi::rule<T, Technique(), ascii::space_type> start;
    };

    template<class T>
    struct FileParser : boost::spirit::qi::grammar<T, File(), ascii::space_type>
    {
        FileParser() : FileParser::base_type(start)
        {
            statement %=
                (
                    techniqueParser |
                    functionParser |
                    (variableParser >> ';')
                    //';'
                )
            ;
            start =
                *(statement[push_back(at_c<0>(_val), _1)])
            ;
        }

        VariableParser<T> variableParser;
        FunctionParser<T> functionParser;
        TechniqueParser<T> techniqueParser;

        qi::rule<T, Hlsl::GlobalStatement(), ascii::space_type> statement;
        qi::rule<T, File(), ascii::space_type> start;
    };
}

template<class TIterator>
bool parse_hlsl_file(TIterator first, TIterator last)
{
    using boost::phoenix::ref;

}

template<class TOut>
void printVariable(TOut& out, Hlsl::Variable const& var)
{
    out << "Variable: " << var.type << " / " << var.name << " / " << var.semantic << " / ";
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
        sampler2D toto = sampler_state {};

        float4 toto(float4 bibi : POSITION, float4x4 toto : TEXCOORD1) : POSITION
        {
            toto;
            titi;
            for (int i = 10; i < 10; i++)
            {
                test();
            }
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

    File file;
    if (qi::phrase_parse(tmp.begin(), tmp.end(), FileParser<std::string::iterator>(), ascii::space, file))
    {
        std::cout << "file: " << std::endl;
        for (auto& s: file.statements)
            switch (s.which()) //<Hlsl::Variable, Hlsl::Function, Hlsl::Technique>;
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
                        std::cout << "\tPass: " << pass.name;
                }
                break;
            }
    }

    std::cin.get();
    return 0;
}
