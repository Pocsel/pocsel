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
        }

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
