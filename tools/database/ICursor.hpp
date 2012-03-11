#ifndef __TOOLS_DATABASE_ICURSOR_HPP__
#define __TOOLS_DATABASE_ICURSOR_HPP__

#include "tools/database/IRow.hpp"
#include "tools/database/IStatement.hpp"
#include "tools/database/RowIterator.hpp"

namespace Tools { namespace Database {

    class RowIterator;

    class ICursor
    {
    public:
        virtual IStatement& Execute(char const* req) = 0;
        virtual IStatement* Prepare(char const* req) = 0;
        IStatement& Execute(std::string const& req) { return this->Execute(req.c_str()); } // du coup c'est plus une interface
        IStatement* Prepare(std::string const& req) { return this->Prepare(req.c_str()); } // du coup c'est plus une interface
        virtual IRow& FetchOne() = 0;
        virtual RowIterator FetchAll() = 0;
        virtual bool HasData() = 0;
        virtual Uint64 LastRowId() = 0;
        virtual ~ICursor()
        {
        }
    };

}}

#endif
