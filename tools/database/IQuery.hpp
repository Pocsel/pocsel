#ifndef __TOOLS_DATABASE_IQUERY_HPP__
#define __TOOLS_DATABASE_IQUERY_HPP__

#include "tools/database/IRow.hpp"

namespace Tools { namespace Database {

    class IQuery
    {
    public:
        virtual ~IQuery() {}
        virtual IQuery& Bind(Int8 val) = 0;
        virtual IQuery& Bind(Uint8 val) = 0;
        virtual IQuery& Bind(Int16 val) = 0;
        virtual IQuery& Bind(Uint16 val) = 0;
        virtual IQuery& Bind(Int32 val) = 0;
        virtual IQuery& Bind(Uint32 val) = 0;
        virtual IQuery& Bind(Int64 val) = 0;
        virtual IQuery& Bind(Uint64 val) = 0;
        virtual IQuery& Bind(float val) = 0;
        virtual IQuery& Bind(double val) = 0;
        virtual IQuery& Bind(std::string const& val) = 0;
        virtual IQuery& Bind(void const* tab, std::size_t size) = 0;
        virtual IQuery& ExecuteNonSelect() = 0;
        virtual std::unique_ptr<IRow> Fetch() = 0;
        virtual void Reset() = 0;
    };

}}

#endif
