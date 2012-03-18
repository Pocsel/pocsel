#ifndef __DOC_TOTO_HPP__
#define __DOC_TOTO_HPP__

#include <string>

#include "bite.hpp"

namespace Doc
{

    class Toto
    {
    public:
        typedef unsigned int Node;
    protected:
        typedef unsigned int _Graph;
    private:
        typedef unsigned int _Graph2;

    public:
        std::string pub;
    protected:
        std::string _prot;
    private:
        std::string _privVar;

    public:
        explicit Toto();
        ~Toto();
        void operator ()();
        std::string const& GetProt() const
        {
            int a = 0;
            Parent::GetProt();
            return this->_prot;
        }
    protected:
        std::string _CalcDot() const;
    private:
        std::string _CalcGraphViz() const;
    };

}

#endif
