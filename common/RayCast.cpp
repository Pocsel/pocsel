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

        void __Ray(Common::Camera const& cam, float distance,
                   std::map<double, Tools::Vector3i>& res)
        {
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

        template<typename T>
        std::vector<Common::CubePosition> __MapToTab(std::map<T, Tools::Vector3i> const& preRes, Common::Position const& pos)
        {
            Common::BaseChunk::CoordsType world;
            Tools::Vector3u chunk;

            std::vector<Common::CubePosition> res(preRes.size());

            for (auto mit = preRes.begin(), mite = preRes.end(); mit != mite; ++mit)
            {
                world = pos.world;

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

    std::vector<Common::CubePosition> RayCast::Ray(Common::Camera const& cam, float distance)
    {
        std::map<double, Tools::Vector3i> preRes;

        __Ray(cam, distance, preRes);


        return __MapToTab(preRes, cam.position);
    }

    namespace {

        void __SphereArea(Common::Position const& pos, float distance,
                    std::vector<Tools::Vector3i>& res)
        {
            double sqdist = (double)distance * (double)distance;

            res.reserve((std::size_t)(distance*distance*distance*boost::math::constants::pi<double>()*4.0/3.0));
            for (double x = -distance; x < distance; x += 1)
            {
                for (double y = -distance; y < distance; y += 1)
                {
                    for (double z = -distance; z < distance; z += 1)
                    {
                        if (x*x+y*y+z*z < sqdist)
                            res.push_back(Tools::Vector3i(x + pos.chunk.x, y + pos.chunk.y, z + pos.chunk.z));
                    }
                }
            }
        }

        void __CubeArea(Common::Position const& pos, float distance,
                    std::vector<Tools::Vector3i>& res)
        {
            res.reserve((std::size_t)(distance*distance*distance*8));
            for (double x = -distance; x < distance; x += 1)
            {
                for (double y = -distance; y < distance; y += 1)
                {
                    for (double z = -distance; z < distance; z += 1)
                    {
                        res.push_back(Tools::Vector3i(x + pos.chunk.x, y + pos.chunk.y, z + pos.chunk.z));
                    }
                }
            }
        }

        std::vector<Common::CubePosition> __TabToTab(std::vector<Tools::Vector3i> const& preRes, Common::Position const& pos)
        {
            std::vector<Common::CubePosition> res(preRes.size());

            Common::BaseChunk::CoordsType world;
            Tools::Vector3u chunk;


            for (auto mit = preRes.begin(), mite = preRes.end(); mit != mite; ++mit)
            {
                world = pos.world;

                Tools::Vector3i r = *mit;

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

    std::vector<Common::CubePosition> RayCast::SphereArea(Common::Position const& pos, float distance)
    {
        std::vector<Tools::Vector3i> preRes;

        __SphereArea(pos, distance, preRes);

        return __TabToTab(preRes, pos);
    }

    std::vector<Common::CubePosition> RayCast::CubeArea(Common::Position const& pos, float distance)
    {
        std::vector<Tools::Vector3i> preRes;

        __CubeArea(pos, distance, preRes);

        return __TabToTab(preRes, pos);
    }

}
