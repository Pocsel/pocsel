#ifndef __SV_PACKETEXTRACTOR_HPP__
#define __SV_PACKETEXTRACTOR_HPP__

#include "tools/ByteArray.hpp"

namespace sv {

    class PacketExtractor
    {
    public:
        static void UdpReady(Tools::ByteArray const& p, bool& ready)
        {
            p.Read(ready);
            std::cout << "extract UdpReady | " << ready << " |\n";
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

