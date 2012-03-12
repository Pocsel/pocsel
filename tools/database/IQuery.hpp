#ifndef __TOOLS_DATABASE_IQUERY_HPP__
#define __TOOLS_DATABASE_IQUERY_HPP__

#include "tools/database/IRow.hpp"

namespace Tools { namespace Database {

    class IQuery
    {
    public:
        virtual ~IQuery() {}
        virtual IQuery& Bind(char val) = 0;
        virtual IQuery& Bind(unsigned char val) = 0;
        virtual IQuery& Bind(short val) = 0;
        virtual IQuery& Bind(unsigned short val) = 0;
        virtual IQuery& Bind(int val) = 0;
        virtual IQuery& Bind(unsigned int val) = 0;
        virtual IQuery& Bind(long long val) = 0;
        virtual IQuery& Bind(unsigned long long val) = 0;
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
