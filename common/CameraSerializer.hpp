#ifndef __COMMON_CAMERASERIALIZER_HPP__
#define __COMMON_CAMERASERIALIZER_HPP__

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
            float phi, theta;
            p.Read(pos);
            p.Read(phi);
            p.Read(theta);

            return std::unique_ptr<Camera>(
                    new Camera(pos, phi, theta)
                    );
        }

        static void Write(Camera const& cam, Tools::ByteArray& p)
        {
            p.Write(cam.position);
            p.Write(cam.phi);
            p.Write(cam.theta);
        }

        static void Read(Tools::ByteArray const& p, Camera& cam)
        {
            p.Read(cam.position);
            float phi, theta;
            p.Read(phi);
            p.Read(theta);
            cam.SetAngles(phi, theta);
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
