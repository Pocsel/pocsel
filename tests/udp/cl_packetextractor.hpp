#ifndef __CL_PACKETEXTRACTOR_HPP__
#define __CL_PACKETEXTRACTOR_HPP__

#include "tools/ByteArray.hpp"

namespace cl {

    class PacketExtractor
    {
    public:
        static void Login(Tools::ByteArray const& p, Uint32& id)
        {
            p.Read(id);
            std::cout << "extract Login | " << id << " |\n";
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
