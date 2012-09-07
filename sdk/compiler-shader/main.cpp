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
    struct Identifier
    {
        std::string name;
    };

    // mul(worldMatrix, float4(normal, 0.0))
    struct Statement
    {
        std::string statement;
    };

    // float3 normal : NORMAL
    // float4 position : POSITION
    struct Variable
    {
        Identifier type;
        Identifier name;
        Identifier semantic;
        Statement value;
    };

    // void vertexShader(float4 position : POSITION, float2 texCoord : TEXCOORD) { ... }
    struct Function
    {
        Identifier ret;
        Identifier name;
        std::list<Variable> arguments;
        std::list<Statement> statements;
    };

    // VertexShader = vertexShader;
    // AlphaBlendEnable = false;
    struct PassStatement
    {
        Identifier key;
        std::string value;
    };

    // pass Toto { VertexShader = vertexShader; PixelShader = pixelShader; }
    struct Pass
    {
        Identifier name;
        std::list<PassStatement> statements;
    };

    // technique Toto { pass { VertexShader = vertexShader; PixelShader = pixelShader; }}
    struct Technique
    {
        Identifier name;
        std::list<Pass> passes;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Identifier,
    (std::string, name)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Statement,
    (std::string, statement)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Variable,
    (Hlsl::Identifier, type)
    (Hlsl::Identifier, name)
    (Hlsl::Identifier, semantic)
    (Hlsl::Statement, value)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Function,
    (Hlsl::Identifier, ret)
    (Hlsl::Identifier, name)
    (std::list<Hlsl::Variable>, arguments)
    (std::list<Hlsl::Statement>, statements)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::PassStatement,
    (Hlsl::Identifier, key)
    (Hlsl::Identifier, value)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Pass,
    (Hlsl::Identifier, name)
    (std::list<Hlsl::PassStatement>, statements)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Technique,
    (Hlsl::Identifier, name)
    (std::list<Hlsl::Pass>, passes)
)

namespace Hlsl {

    namespace ascii = boost::spirit::ascii;
    namespace qi = boost::spirit::qi;
    using qi::char_;
    using qi::lit;
    using ascii::string;
    using namespace qi::labels;

    template<class T>
    struct IdentifierParser : boost::spirit::qi::grammar<T, Identifier()>
    {
        IdentifierParser() : IdentifierParser::base_type(start)
        {
            parser %= char_("_A-Za-z") >> *char_("_A-Za-z0-9");

            start %= parser;
        }

        boost::spirit::qi::rule<T, std::string()> parser;
        boost::spirit::qi::rule<T, Identifier()> start;
    };

    template<class T>
    struct StatementParser : boost::spirit::qi::grammar<T, Statement()>
    {
        StatementParser() : StatementParser::base_type(start)
        {
            parser %= (char_ - ';') >> *(char_ - ';');

            start %= parser;
        }

        boost::spirit::qi::rule<T, std::string()> parser;
        boost::spirit::qi::rule<T, Statement()> start;
    };

    template<class T>
    struct VariableParser : boost::spirit::qi::grammar<T, Variable(), ascii::space_type>
    {
        VariableParser() : VariableParser::base_type(start)
        {
            semanticParser = -(':' << identifierParser[_val = _1]);

            valueParser =
                    ('=' << statementParser[_val = _1])
                    |
                    -';'
                ;

            start %=
                    identifierParser // type
                    << identifierParser // name
                    << semanticParser // semantic
                    << valueParser // value
                ;
        }

        IdentifierParser<T> identifierParser;
        StatementParser<T> statementParser;
        boost::spirit::qi::rule<T, Identifier()> semanticParser;
        boost::spirit::qi::rule<T, Statement()> valueParser;
        boost::spirit::qi::rule<T, Variable(), ascii::space_type> start;
    };
}

template<class TIterator>
bool parse_hlsl_file(TIterator first, TIterator last)
{
    using boost::phoenix::ref;

}

int main(int ac, char** av)
{
    std::vector<std::string> test;
    parse_numbers("coucou123", test);
    parse_numbers("321", test);
    parse_numbers("titi", test);
    for (auto& v : test)
    {
        std::cout << v << std::endl;
    }

    std::complex<double> c;
    std::string tmp("(12.3, 32.4)");
    parse_complex(tmp.begin(), tmp.end(), c);
    std::cout << c << std::endl;

    using namespace Hlsl;
    IdentifierParser<std::string::iterator> gIdentifier;
    Identifier type;
    tmp = "sampler2D";
    auto r = boost::spirit::qi::phrase_parse(tmp.begin(), tmp.end(), gIdentifier, boost::spirit::ascii::space, type);
    if (r)
        std::cout << "Identifier::name: " << type.name << std::endl;

    VariableParser<std::string::iterator> gVariable;
    Variable var;
    tmp = "float4x4 toto : WorldViewProjection = mul(a, b);";
    r = boost::spirit::qi::phrase_parse(tmp.begin(), tmp.end(), gVariable, boost::spirit::ascii::space, var);
    if (r)
        std::cout << "Variable:" << var.type.name << " / " << var.name.name << " / " << var.semantic.name << " / " << var.value.statement << std::endl;

    std::cin.get();
    return 0;
}
