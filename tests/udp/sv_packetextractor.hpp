#ifndef __SV_PACKETEXTRACTOR_HPP__
#define __SV_PACKETEXTRACTOR_HPP__

#include "tools/ByteArray.hpp"

namespace sv {

    class PacketExtractor
    {
    public:
        static void UdpReady(Tools::ByteArray const&)
        {
            std::cout << "extract UdpReady\n";
        }

        static void PassThrough(Tools::ByteArray const& p, Uint32& type)
        {
            p.Read(type);
            std::cout << "extract PassThrough | " << type << " |\n";
        }

        static void PassThroughOk(Tools::ByteArray const& p, Uint32& type)
        {
            p.Read(type);
            std::cout << "extract PassThroughOk | " << type << " |\n";
        }
    };

}

#endif

