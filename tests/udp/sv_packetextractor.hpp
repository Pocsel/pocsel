#ifndef __SV_PACKETEXTRACTOR_HPP__
#define __SV_PACKETEXTRACTOR_HPP__

#include "tools/ByteArray.hpp"

namespace sv {

    class PacketExtractor
    {
    public:
        static void PassThrough(Tools::ByteArray const& p, Uint32& type)
        {
            p.Read(type);
        }

        static void PassThroughOk(Tools::ByteArray const& p, Uint32& type)
        {
            p.Read(type);
        }
    };

}

#endif

