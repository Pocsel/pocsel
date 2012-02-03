#ifndef __TOOLS_SQRT_HPP__
#define __TOOLS_SQRT_HPP__

/*
 * Les sqrt manquant dans std::
 */

#ifdef _WIN32
# pragma warning(push)
# pragma warning(disable: 4244)
#endif

namespace std {

    inline Int8 sqrt(Int8 val)
    {
        return std::sqrt(static_cast<float>(val));
    }
    inline Int16 sqrt(Int16 val)
    {
        return std::sqrt(static_cast<float>(val));
    }
    inline Int32 sqrt(Int32 val)
    {
        return std::sqrt(static_cast<float>(val));
    }
    inline Int64 sqrt(Int64 val)
    {
        return std::sqrt(static_cast<double>(val));
    }
    inline Uint8 sqrt(Uint8 val)
    {
        return std::sqrt(static_cast<float>(val));
    }
    inline Uint16 sqrt(Uint16 val)
    {
        return std::sqrt(static_cast<float>(val));
    }
    inline Uint32 sqrt(Uint32 val)
    {
        return std::sqrt(static_cast<float>(val));
    }
    inline Uint64 sqrt(Uint64 val)
    {
        return std::sqrt(static_cast<double>(val));
    }
}

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif
