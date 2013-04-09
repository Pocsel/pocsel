#include "sdk/compiler-shader/HlslParser.hpp"

BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Statement,
    (std::string, statement)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::CodeBlock,
    (std::list<Hlsl::StatementOrCodeBlock>, statements)
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
    (std::string, arraySize)
    (std::string, semantic)
    (Hlsl::StatementOrSamplerState, value)
    (bool, in)
    (bool, out)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Structure,
    (std::string, name)
    (std::list<Hlsl::Variable>, members)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Function,
    (std::string, ret)
    (std::string, name)
    (std::list<Hlsl::Variable>, arguments)
    (std::string, semantic)
    (Hlsl::CodeBlock, code)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::PassStatement,
    (std::string, key)
    (std::string, value)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::CompileStatement,
    (Hlsl::CompileStatement::ShaderType, type)
    (std::string, entry)
    (std::string, profile)
)
BOOST_FUSION_ADAPT_STRUCT(
    Hlsl::Pass,
    (std::string, name)
    (std::list<Hlsl::PassOrCompileStatement>, statements)
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
    using boost::phoenix::construct;
    using boost::phoenix::at_c;
    using boost::phoenix::push_back;
    using boost::spirit::_1;

    struct ErrorHandler
    {
        template<class T1 = void, class T2 = void, class T3 = void, class T4 = void, class T5 = void>
        struct result
        {
            typedef void type;
        };

        template<class T1, class T2, class T3, class T4>
        void operator()(T1 const& it, T2 const& itEnd, T3 const& itError, T4 const& info)
        {
            std::cerr << " *** FATAL *** Expecting " << info << " near: " << std::string(it, itError) << std::endl;
        }
    };

    template<class T>
    struct BaseParser
    {
        qi::rule<T, std::string()> identifier;
        qi::rule<T, void()> leftBracket;
        qi::rule<T, void()> rightBracket;
        qi::rule<T, void()> leftParenthesis;
        qi::rule<T, void()> rightParenthesis;
        qi::rule<T, void()> semicolon;
        qi::rule<T, void()> colon;
        qi::rule<T, void()> equal;
        qi::rule<T, void()> comma;
        boost::phoenix::function<ErrorHandler> errorHandler;

        BaseParser();
    };

    template<class T>
    BaseParser<T>::BaseParser()
    {
        identifier %= char_("_A-Za-z") >> *char_("_A-Za-z0-9");
        identifier.name("identifier");

        leftBracket = lit('{');
        leftBracket.name("{");

        rightBracket = lit('}');
        rightBracket.name("}");

        leftParenthesis = lit('(');
        leftParenthesis.name("(");

        rightParenthesis = lit(')');
        rightParenthesis.name(")");

        semicolon = lit(';');
        semicolon.name(";");

        colon = lit(':');
        colon.name(":");

        equal = lit('=');
        equal.name("=");

        comma = lit(',');
        comma.name(",");
    }

    template<class T, class TSkipper>
    struct DeviceStateParser : boost::spirit::qi::grammar<T, DeviceState(), TSkipper>
    {
        BaseParser<T> base;
        qi::rule<T, DeviceState(), TSkipper> start;

        DeviceStateParser() : DeviceStateParser::base_type(start)
        {
            start %= base.identifier >> base.equal >> +char_("_A-Za-z0-9");
        }
    };

    template<class T, class TSkipper>
    struct PassStatementParser : boost::spirit::qi::grammar<T, PassStatement(), TSkipper>
    {
        BaseParser<T> base;
        qi::rule<T, PassStatement(), TSkipper> start;

        PassStatementParser() : PassStatementParser::base_type(start)
        {
            start %= base.identifier > base.equal > +char_("()[]_A-Za-z0-9") > base.semicolon;
        }
    };

    template<class T, class TSkipper>
    struct CompileStatementParser : boost::spirit::qi::grammar<T, CompileStatement(), TSkipper>
    {
        BaseParser<T> base;
        qi::rule<T, CompileStatement(), TSkipper> start;

        CompileStatementParser() : CompileStatementParser::base_type(start)
        {
            start =
                    (
                        lit("VertexShader")[at_c<0>(_val) = val(CompileStatement::VertexShader)]
                        |
                        lit("VertexProgram")[at_c<0>(_val) = val(CompileStatement::VertexShader)]
                        |
                        lit("PixelShader")[at_c<0>(_val) = val(CompileStatement::PixelShader)]
                        |
                        lit("FragmentProgram")[at_c<0>(_val) = val(CompileStatement::PixelShader)]
                    )
                    > base.equal
                    > "compile"
                    > base.identifier[at_c<2>(_val) = _1]
                    > base.identifier[at_c<1>(_val) = _1]
                    > base.leftParenthesis
                    > qi::lexeme[*(char_ - '(' - ')' - ';')]
                    > base.rightParenthesis
                    > base.semicolon
                ;
        }
    };

    template<class T, class TSkipper>
    struct StatementParser : boost::spirit::qi::grammar<T, Statement(), TSkipper>
    {
        BaseParser<T> base;
        qi::rule<T, Statement(), TSkipper> start;

        StatementParser() : StatementParser::base_type(start)
        {
            start = qi::lexeme[+(char_ - '{' - '}' - ';')[at_c<0>(_val) += _1]];
        }
    };

    template<class T, class TSkipper>
    struct CodeBlockParser : boost::spirit::qi::grammar<T, CodeBlock(), TSkipper>
    {
        BaseParser<T> base;
        StatementParser<T, TSkipper> statement;
        qi::rule<T, Statement(), TSkipper> parser;
        qi::rule<T, CodeBlock(), TSkipper> start;

        CodeBlockParser() : CodeBlockParser::base_type(start)
        {
            parser = statement[_val = _1] > -(base.semicolon[at_c<0>(_val) += val(';')]);

            start =
                    base.leftBracket
                    >> *(
                            start[push_back(at_c<0>(_val), _1)]
                            |
                            parser[push_back(at_c<0>(_val), _1)]
                        )
                    >> base.rightBracket
                ;
        }
    };

    template<class T, class TSkipper>
    struct VariableParser : boost::spirit::qi::grammar<T, Variable(), TSkipper>
    {
        BaseParser<T> base;
        DeviceStateParser<T, TSkipper> deviceStateParser;
        StatementParser<T, TSkipper> statementParser;
        boost::spirit::qi::rule<T, std::string(), TSkipper> semanticParser;
        boost::spirit::qi::rule<T, std::string(), TSkipper> arraySizeParser;
        boost::spirit::qi::rule<T, SamplerState(), TSkipper> samplerStateParser;
        boost::spirit::qi::rule<T, StatementOrSamplerState(), TSkipper> valueParser;
        boost::spirit::qi::rule<T, Variable(), TSkipper> start;

        VariableParser() : VariableParser::base_type(start)
        {
            semanticParser %= (base.colon > base.identifier) | qi::eps[_val = val("")];

            arraySizeParser %=
                    (
                        lit('[')
                        >  *char_("0-9")
                        > lit(']')
                    )
                    | qi::eps[_val = val("")]
                ;

            samplerStateParser %= lit("sampler_state") > base.leftBracket > *(deviceStateParser >> base.semicolon) > base.rightBracket;
            qi::on_error<qi::rethrow>(samplerStateParser, base.errorHandler);

            valueParser =
                    -(
                        base.equal
                        > (samplerStateParser[_val = _1] | statementParser[_val = _1])
                    )
                    | qi::eps[_val = val(Statement())]
                ;
            qi::on_error<qi::rethrow>(valueParser, base.errorHandler);

            start %=
                    base.identifier[if_(_1 == "in" || _1 == "out" || _1 == "inout")[_pass = false]] // type
                    >> base.identifier // name
                    >> arraySizeParser // arraySize
                    >> semanticParser // semantic
                    >> valueParser // value
                ;
        }
    };

    template<class T, class TSkipper>
    struct StructureParser : boost::spirit::qi::grammar<T, Structure(), TSkipper>
    {
        BaseParser<T> base;
        VariableParser<T, TSkipper> variableParser;
        boost::spirit::qi::rule<T, Structure(), TSkipper> start;

        StructureParser() : StructureParser::base_type(start)
        {
            start %=
                    "struct"
                    > base.identifier // name
                    > base.leftBracket
                    > +(variableParser > ';')
                    > base.rightBracket
                    > base.semicolon
                ;
        }
    };

    template<class T, class TSkipper>
    struct FunctionParser : boost::spirit::qi::grammar<T, Function(), TSkipper>
    {
        BaseParser<T> base;
        CodeBlockParser<T, TSkipper> codeBlockParser;
        VariableParser<T, TSkipper> variableParser;
        boost::spirit::qi::rule<T, Variable(), TSkipper> oneArgParser;
        boost::spirit::qi::rule<T, std::list<Variable>(), TSkipper> argumentParser;
        boost::spirit::qi::rule<T, std::string(), TSkipper> semanticParser;
        boost::spirit::qi::rule<T, Function(), TSkipper> start;

        FunctionParser() : FunctionParser::base_type(start)
        {
            semanticParser %= (base.colon >> base.identifier) | qi::eps[_val = val("")];

            oneArgParser =
                    (lit("in") >> variableParser[_val = _1])
                    | (lit("out") >> variableParser[_val = _1, at_c<5>(_val) = val(false), at_c<6>(_val) = val(true)])
                    | (lit("inout") >> variableParser[_val = _1, at_c<5>(_val) = val(true), at_c<6>(_val) = val(true)])
                    | variableParser[_val = _1]
                ;
            oneArgParser.name("argument");

            argumentParser =
                    base.leftParenthesis
                    > -(oneArgParser[push_back(_val, _1)] % base.comma)
                    > base.rightParenthesis
                ;
            argumentParser.name("arguments");
            qi::on_error<qi::rethrow>(argumentParser, base.errorHandler);

            start %=
                    base.identifier // type
                    >> base.identifier // name
                    >> argumentParser
                    >> semanticParser // semantic
                    >> codeBlockParser
                ;
        }
    };

    template<class T, class TSkipper>
    struct TechniqueParser : boost::spirit::qi::grammar<T, Technique(), TSkipper>
    {
        BaseParser<T> base;
        PassStatementParser<T, TSkipper> passStatementParser;
        CompileStatementParser<T, TSkipper> compileStatementParser;
        boost::spirit::qi::rule<T, Pass(), TSkipper> passParser;
        boost::spirit::qi::rule<T, Technique(), TSkipper> start;

        TechniqueParser() : TechniqueParser::base_type(start)
        {
            auto statement =
                    compileStatementParser[push_back(at_c<1>(_val), _1)]
                    |
                    passStatementParser[push_back(at_c<1>(_val), _1)]
                ;

            passParser =
                    "pass"
                    > base.identifier[at_c<0>(_val) = _1] // name
                    > base.leftBracket
                    > *statement
                    > base.rightBracket
                ;
            passParser.name("pass");

            start =
                    "technique"
                    > base.identifier[at_c<0>(_val) = _1] // name
                    > base.leftBracket
                    > *(passParser[push_back(at_c<1>(_val), _1)])
                    > base.rightBracket
                ;
        }
    };

    template<class T, class TSkipper>
    struct FileParser : boost::spirit::qi::grammar<T, File(), TSkipper>
    {
        BaseParser<T> base;
        VariableParser<T, TSkipper> variableParser;
        StructureParser<T, TSkipper> structureParser;
        FunctionParser<T, TSkipper> functionParser;
        TechniqueParser<T, TSkipper> techniqueParser;

        qi::rule<T, Hlsl::GlobalStatement(), TSkipper> statement;
        qi::rule<T, Variable(), TSkipper> variable;
        qi::rule<T, File(), TSkipper> start;

        FileParser();
    };

    template<class T, class TSkipper>
    FileParser<T, TSkipper>::FileParser() : FileParser::base_type(start)
    {
        variable =
            variableParser[_val = _1] > base.semicolon
        ;

        statement =
            techniqueParser[_val = _1] |
            structureParser[_val = _1] |
            functionParser[_val = _1] |
            variable[_val = _1] |
            base.semicolon[_val = val(Nil())]
        ;
        statement.name("statement");

        start =
            *statement[push_back(at_c<0>(_val), _1)]
            > qi::eoi;
        ;
        start.name("file");
    }

    bool ParseStream(std::istream& in, File& file, std::list<std::string> const& macros, std::ostream& errors)
    {
        typedef std::istreambuf_iterator<char> base_it;
        base_it tmpIt(in);
        std::string tmp(tmpIt, base_it());

        typedef boost::wave::cpplexer::lex_iterator<boost::wave::cpplexer::lex_token<>> lex_iterator_type;
        typedef boost::wave::context<std::string::iterator, lex_iterator_type> context_type;
        context_type ctx(tmp.begin(), tmp.end(), "tmp.fx");
        for (auto const& macro: macros)
            ctx.add_macro_definition(macro);
        std::stringstream ss;
        for (auto& it: ctx)
            ss << it.get_value();
        tmp = ss.str();

        auto it = tmp.begin(); //boost::spirit::make_default_multi_pass(tmpIt);
        auto itEnd = tmp.end(); //boost::spirit::make_default_multi_pass(base_it());

        auto skip =
            ascii::space |
            ("//" >> *(char_ - qi::eol - qi::eoi) >> -qi::eol) |
            ("#line" >> *ascii::space >> +ascii::alnum >> *ascii::space >> '"' >> *(char_ - '"') >> '"') | //#line 49 "D:\\Programmation\\C++\\pocsel\\build_x64_vs11\\sdk\\compiler-shader\\tmp.fx"
            ("/*" >> *(char_ - "*/") >> "*/")
        ;

        FileParser<decltype(it), decltype(skip)> fileParser;
        try
        {
            return qi::phrase_parse(it, itEnd, fileParser, skip, file);
        }
        catch (qi::expectation_failure<decltype(it)>& ex)
        {
            errors << "ERROR: expecting " << ex.what_ << " near:\n" << std::string(ex.first, ex.last) << std::endl;
        }
        return false;
    }

    std::ostream& operator <<(std::ostream& out, GlobalStatement const& stmt)
    {
        // typedef boost::variant<Hlsl::Variable, Hlsl::Function, Hlsl::Technique, Hlsl::Structure, Hlsl::Nil> GlobalStatement;
        struct : public boost::static_visitor<std::string>
        {
            std::string operator()(Variable const& var)
            {
                return "variable<" + var.name + ">";
            }
            std::string operator()(Function const& func)
            {
                return "function<" + func.name + ">";
            }
            std::string operator()(Technique const& tech)
            {
                return "technique<" + tech.name + ">";
            }
            std::string operator()(Structure const& struc)
            {
                return "structure<" + struc.name + ">";
            }
            std::string operator()(Nil const&)
            {
                return "nil";
            }
        } visitor;
        return out << boost::apply_visitor(visitor, stmt);
    }
}
