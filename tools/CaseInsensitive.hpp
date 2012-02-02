#ifndef __TOOLS_CASEINSENSITIVE_HPP__
#define __TOOLS_CASEINSENSITIVE_HPP__

#include <boost/algorithm/string/predicate.hpp>

// a utiliser en troisieme argument template de std::map
// std::map<string, osef, Tools::CaseInsensitive>

namespace Tools {

    struct CaseInsensitive
    {
        bool operator()(std::string const& s1, std::string const& s2)
        {
            return boost::algorithm::ilexicographical_compare(s1, s2);
        }
    };

}

#endif
