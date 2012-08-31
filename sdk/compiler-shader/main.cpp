#include "precompiled.hpp"


bool parse_numbers(char const* str, std::vector<std::string>& v)
{
    using namespace boost::spirit;

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

    std::cin.get();
    return 0;
}
