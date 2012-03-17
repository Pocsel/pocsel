#ifndef __COMMON_MOVINGORIENTEDPOSITIONSERIALIZER_HPP__
#define __COMMON_MOVINGORIENTEDPOSITIONSERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/VectorSerializer.hpp"

#include "common/MovingOrientedPosition.hpp"
#include "common/OrientedPositionSerializer.hpp"

namespace Common {

    struct MovingOrientedPositionSerializer
    {
        static std::unique_ptr<MovingOrientedPosition> Read(Tools::ByteArray const& p)
        {
            Common::OrientedPosition pos;
            Tools::Vector3f movement;
            p.Read(pos);
            p.Read(movement);

            return std::unique_ptr<MovingOrientedPosition>(
                    new MovingOrientedPosition(pos, movement)
                    );
        }

        static void Write(MovingOrientedPosition const& pos, Tools::ByteArray& p)
        {
            p.Write(pos.position);
            p.Write(pos.movement);
        }

        static void Read(Tools::ByteArray const& p, MovingOrientedPosition& pos)
        {
            p.Read(pos.position);
            p.Read(pos.movement);
        }
    };

}

namespace Tools {

    template<> struct ByteArray::Serializer< ::Common::MovingOrientedPosition > :
        public ::Common::MovingOrientedPositionSerializer
    {
    };

}

#endif

