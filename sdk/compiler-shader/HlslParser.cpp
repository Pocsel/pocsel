// Boost
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/lex.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

#include "sdk/compiler-shader/HlslParser.hpp"

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

    template<class T, class TSkipper>
    struct DeviceStateParser : boost::spirit::qi::grammar<T, DeviceState(), TSkipper>
    {
        DeviceStateParser() : DeviceStateParser::base_type(start)
        {
            start %= identifierParser.parser >> "=" >> identifierParser.parser;
        }

        IdentifierParser<T> identifierParser;
        qi::rule<T, DeviceState(), TSkipper> start;
    };

    template<class T, class TSkipper>
    struct PassStatementParser : boost::spirit::qi::grammar<T, PassStatement(), TSkipper>
    {
        PassStatementParser() : PassStatementParser::base_type(start)
        {
            start %= identifierParser.parser >> "=" >> *(char_ - ';') >> ';';
        }

        IdentifierParser<T> identifierParser;
        qi::rule<T, PassStatement(), TSkipper> start;
    };

    template<class T, class TSkipper>
    struct StatementParser : boost::spirit::qi::grammar<T, Statement(), TSkipper>
    {
        StatementParser() : StatementParser::base_type(start)
        {
            parser %= *(char_ - '{' - '}' - ';');

            start = parser[at_c<0>(_val) += _1] >> -(char_('{')[at_c<0>(_val) += _1] >> *(parser[at_c<0>(_val) += _1] >> ';') >> char_('}')[at_c<0>(_val) += _1]);
        }

        qi::rule<T, std::string()> parser;
        qi::rule<T, Statement(), TSkipper> start;
    };

    template<class T, class TSkipper>
    struct VariableParser : boost::spirit::qi::grammar<T, Variable(), TSkipper>
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
        DeviceStateParser<T, TSkipper> deviceStateParser;
        StatementParser<T, TSkipper> statementParser;
        boost::spirit::qi::rule<T, std::string(), TSkipper> semanticParser;
        boost::spirit::qi::rule<T, SamplerState(), TSkipper> samplerStateParser;
        boost::spirit::qi::rule<T, StatementOrSamplerState(), TSkipper> valueParser;
        boost::spirit::qi::rule<T, Variable(), TSkipper> start;
    };

    template<class T, class TSkipper>
    struct FunctionParser : boost::spirit::qi::grammar<T, Function(), TSkipper>
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
        StatementParser<T, TSkipper> statementParser;
        VariableParser<T, TSkipper> variableParser;
        boost::spirit::qi::rule<T, std::list<Variable>(), TSkipper> argumentParser;
        boost::spirit::qi::rule<T, std::list<Statement>(), TSkipper> statementsParser;
        boost::spirit::qi::rule<T, std::string(), TSkipper> semanticParser;
        boost::spirit::qi::rule<T, Function(), TSkipper> start;
    };

    template<class T, class TSkipper>
    struct TechniqueParser : boost::spirit::qi::grammar<T, Technique(), TSkipper>
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
        PassStatementParser<T, TSkipper> passStatementParser;
        boost::spirit::qi::rule<T, Pass(), TSkipper> passParser;
        boost::spirit::qi::rule<T, Technique(), TSkipper> start;
    };

    template<class T, class TSkipper>
    struct FileParser : boost::spirit::qi::grammar<T, File(), TSkipper>
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
                >> qi::eoi;
            ;
        }

        VariableParser<T, TSkipper> variableParser;
        FunctionParser<T, TSkipper> functionParser;
        TechniqueParser<T, TSkipper> techniqueParser;

        qi::rule<T, Hlsl::GlobalStatement(), TSkipper> statement;
        qi::rule<T, File(), TSkipper> start;
    };

    bool ParseStream(std::istream& in, File& file, std::ostream& errors)
    {
        typedef std::istreambuf_iterator<char> base_it;
        auto it = boost::spirit::make_default_multi_pass(base_it(in));
        auto itEnd = boost::spirit::make_default_multi_pass(base_it());

        auto skip =
            ascii::space |
            ("//" >> *(char_ - qi::eol - qi::eoi) >> -qi::eol) |
            ("/*" >> *(char_ - "*/") >> "*/")
        ;

        FileParser<decltype(it), decltype(skip)> fileParser;
        return qi::phrase_parse(it, itEnd, fileParser, skip, file);
    }
}
