#ifndef __COMMON_ORIENTEDPOSITIONSERIALIZER_HPP__
#define __COMMON_ORIENTEDPOSITIONSERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/VectorSerializer.hpp"

#include "common/OrientedPosition.hpp"

namespace Common {

    struct OrientedPositionSerializer
    {
        static std::unique_ptr<OrientedPosition> Read(Tools::ByteArray const& p)
        {
            Common::Position pos;
            float theta, phi;
            p.Read(pos);
            p.Read(theta);
            p.Read(phi);

            return std::unique_ptr<OrientedPosition>(
                    new OrientedPosition(pos, theta, phi)
                    );
        }

        static void Write(OrientedPosition const& cam, Tools::ByteArray& p)
        {
            p.Write(cam.position);
            p.Write(cam.theta);
            p.Write(cam.phi);
        }

        static void Read(Tools::ByteArray const& p, OrientedPosition& cam)
        {
            p.Read(cam.position);
            float theta, phi;
            p.Read(theta);
            p.Read(phi);
            cam.SetAngles(theta, phi);
        }
    };

}

namespace Tools {

    template<> struct ByteArray::Serializer< ::Common::OrientedPosition > :
        public ::Common::OrientedPositionSerializer
    {
    };

}

#endif

