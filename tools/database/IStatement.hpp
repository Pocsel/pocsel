#ifndef __TOOLS_DATABASE_ISTATEMENT_HPP__
#define __TOOLS_DATABASE_ISTATEMENT_HPP__

#include "tools/database/IValue.hpp"

namespace Tools { namespace Database {

    class IStatement
    {
    public:
        virtual ~IStatement() {}

        virtual IStatement& Bind(int val) = 0;
        virtual IStatement& Bind(unsigned int val) = 0;
        virtual IStatement& Bind(Int64 val) = 0;
        virtual IStatement& Bind(Uint64 val) = 0;
        virtual IStatement& Bind(float val) = 0;
        virtual IStatement& Bind(double val) = 0;
        virtual IStatement& Bind(bool val) = 0;
        virtual IStatement& Bind(std::string const& s) = 0;
        virtual IStatement& Bind(const char* s) = 0;
        virtual IStatement& Bind(Blob& val) = 0;
        virtual void Reset() = 0;
    };

}}

#endif
