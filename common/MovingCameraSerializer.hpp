#ifndef __COMMON_MOVINGCAMERASERIALIZER_HPP__
#define __COMMON_MOVINGCAMERASERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/VectorSerializer.hpp"

#include "common/Camera.hpp"
#include "common/PositionSerializer.hpp"

namespace Common {

    struct CameraSerializer
    {
        static std::unique_ptr<Camera> Read(Tools::ByteArray const& p)
        {
            Common::Position pos;
            float theta, phi;
            p.Read(pos);
            p.Read(theta);
            p.Read(phi);

            return std::unique_ptr<Camera>(
                    new Camera(pos, theta, phi)
                    );
        }

        static void Write(Camera const& cam, Tools::ByteArray& p)
        {
            p.Write(cam.position);
            p.Write(cam.theta);
            p.Write(cam.phi);
        }

        static void Read(Tools::ByteArray const& p, Camera& cam)
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

    template<> struct ByteArray::Serializer< ::Common::Camera > :
        public ::Common::CameraSerializer
    {
    };

}

#endif

