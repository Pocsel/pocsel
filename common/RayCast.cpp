#include "common/RayCast.hpp"

#include "common/Camera.hpp"
#include "common/CubePosition.hpp"

#define GR_ABS(a) ((a) < 0 ? -(a) : (a))
#define GR_NEGFLOOR(a) ((a) >= 0 ? \
                            (int)(a) \
                        : \
                            (double)(int)(a) == a ? \
                                (int)(a) \
                            : \
                                (int)((a) - 1) \
                        )

namespace Common {

    namespace {

        void __GetRes(Common::Camera const& cam, float distance,
                       std::map<double, Tools::Vector3i>& res)
        {
            if (cam.direction.x == 0)
                return;

            double x, y, z;
            const double
                vx = cam.direction.x,
                vy = cam.direction.y,
                vz = cam.direction.z;
            const double
                px = cam.position.chunk.x,
                pz = cam.position.chunk.z,
                py = cam.position.chunk.y;

            int sign;
            double dist;

            if (vx != 0)
            {
                if (vx < 0)
                {
                    x = (int)px;
                    sign = -1;
                }
                else
                {
                    x = (int)px + 1;
                    sign = 1;
                }

                for (dist = GR_ABS((x - px) / vx); dist < distance ; dist = GR_ABS((x - px) / vx))
                {
                    y = py + (dist * vy);
                    z = pz + (dist * vz);

                    res[dist] = Tools::Vector3i((int)GR_NEGFLOOR(x) - (sign < 0 ? 1 : 0),
                                                (int)GR_NEGFLOOR(y),
                                                (int)GR_NEGFLOOR(z)
                                               );
                    x += sign;
                }
            }

            if (vy != 0)
            {
                if (vy < 0)
                {
                    y = (int)py;
                    sign = -1;
                }
                else
                {
                    y = (int)py + 1;
                    sign = 1;
                }

                for (dist = GR_ABS((y - py) / vy); dist < distance ; dist = GR_ABS((y - py) / vy))
                {
                    x = px + (dist * vx);
                    z = pz + (dist * vz);

                    res[dist] = Tools::Vector3i((int)GR_NEGFLOOR(x),
                                                (int)GR_NEGFLOOR(y) - (sign < 0 ? 1 : 0),
                                                (int)GR_NEGFLOOR(z)
                                               );

                    y += sign;
                }
            }

            if (vz != 0)
            {
                if (vz < 0)
                {
                    z = (int)pz;
                    sign = -1;
                }
                else
                {
                    z = (int)pz + 1;
                    sign = 1;
                }

                for (dist = GR_ABS((z - pz) / vz); dist < distance ; dist = GR_ABS((z - pz) / vz))
                {
                    x = px + (dist * vx);
                    y = py + (dist * vy);

                    res[dist] = Tools::Vector3i((int)GR_NEGFLOOR(x),
                                                (int)GR_NEGFLOOR(y),
                                                (int)GR_NEGFLOOR(z) - (sign < 0 ? 1 : 0)
                                               );

                    z += sign;
                }
            }
        }

    }

    std::vector<Common::CubePosition>RayCast::GetResult(Common::Camera const& cam, float distance)
    {
        std::map<double, Tools::Vector3i> preRes;

        __GetRes(cam, distance, preRes);

        std::vector<Common::CubePosition> res;

        Common::BaseChunk::CoordsType world;
        Tools::Vector3u chunk;


        for (auto mit = preRes.begin(), mite = preRes.end(); mit != mite; ++mit)
        {
            world = cam.position.world;

            Tools::Vector3i r = mit->second;

            while (r.x < 0)
            {
                r.x += Common::ChunkSize;
                world.x -= 1;
            }
            while (r.x >= (int)Common::ChunkSize)
            {
                r.x -= Common::ChunkSize;
                world.x += 1;
            }
            while (r.y < 0)
            {
                r.y += Common::ChunkSize;
                world.y -= 1;
            }
            while (r.y >= (int)Common::ChunkSize)
            {
                r.y -= Common::ChunkSize;
                world.y += 1;
            }
            while (r.z < 0)
            {
                r.z += Common::ChunkSize;
                world.z -= 1;
            }
            while (r.z >= (int)Common::ChunkSize)
            {
                r.z -= Common::ChunkSize;
                world.z += 1;
            }

            chunk.x = r.x;
            chunk.y = r.y;
            chunk.z = r.z;

            res.push_back(Common::CubePosition(world, chunk));
        }

        return res;
    }

}
