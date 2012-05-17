#ifndef __TOOLS_STAT_ISTAT_HPP__
#define __TOOLS_STAT_ISTAT_HPP__

namespace Tools { namespace Stat {

    class IStat
    {
    public:
        virtual ~IStat() {}

        virtual std::string const& GetName() const = 0;
        virtual double GetValue() const = 0;
    };

}}

#endif
