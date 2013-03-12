#ifndef __COMPILER_SHADER_PRECOMPILED_HPP__
#define __COMPILER_SHADER_PRECOMPILED_HPP__

// boost
#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include <boost/variant.hpp>
#include <boost/tuple/tuple.hpp>
// boost - spirit / phoenix
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

#define BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
#include "tools/precompiled.hpp"
#include "tools/logger/Logger.hpp"

// Cg
#include <Cg/cg.h>

#endif
