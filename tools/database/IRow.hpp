#ifndef __TOOLS_DATABASE_IROW_HPP__
#define __TOOLS_DATABASE_IROW_HPP__

#include "IValue.hpp"

namespace Tools { namespace Database {

    class IRow
    {
    public:
        virtual IValue& operator[](unsigned int column) = 0;
        virtual ~IRow() {}
    };

}}

#endif
