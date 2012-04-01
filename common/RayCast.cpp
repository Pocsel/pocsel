#include "common/RayCast.hpp"

#include "common/OrientedPosition.hpp"
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

        void __Ray(Common::OrientedPosition const& pos, float distance,
                   std::map<double, Tools::Vector3i>& res)
        {
            double x, y, z;
            const double
                vx = pos.direction.x,
                vy = pos.direction.y,
                vz = pos.direction.z;
            const double
                px = pos.position.chunk.x,
                pz = pos.position.chunk.z,
                py = pos.position.chunk.y;

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

    std::vector<Common::CubePosition> RayCast::Ray(Common::OrientedPosition const& pos, float distance)
    {
        std::map<double, Tools::Vector3i> preRes;

        __Ray(pos, distance, preRes);


        return __MapToTab(preRes, pos.position);
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
                            res.push_back(Tools::Vector3i((int)(x + pos.chunk.x), (int)(y + pos.chunk.y), (int)(z + pos.chunk.z)));
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
                        res.push_back(Tools::Vector3i((int)(x + pos.chunk.x), (int)(y + pos.chunk.y), (int)(z + pos.chunk.z)));
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

        unsigned int __GetCubePos(int i)
        {
            while (i < 0)
                i += ChunkSize;
            i %= ChunkSize;
            return i;
        }
        unsigned int __GetChunkPos(int i, unsigned int start)
        {
            std::cout << "i=" << i << ", start=" << start << "\n";
            while (i < 0)
            {
                start -= 1;
                i += ChunkSize;
            }
            start += i / ChunkSize;
            return start;
        }
    }

    std::vector<CastChunk*> RayCast::CubeArea(Common::Position const& pos, float distance)
    {
        std::vector<CastChunk*> res;
        res.reserve((std::size_t)((distance*distance*distance)*8/(ChunkSize3)));

        unsigned int cx, cy, cz;//, bx, by, bz;
        int zx, zy, zz, dx, dy, dz;
        double x, y, z;

        x = -distance;
        while (x < distance)
        {
            std::cout << "x=" << x << ", ";
            cx = __GetChunkPos(x + (int)pos.chunk.x, pos.world.x);
            std::cout << "cx=" << cx << "\n";
            y = -distance;
            while (y < distance)
            {
                cy = __GetChunkPos(y + (int)pos.chunk.y, pos.world.y);
                std::cout << "y=" << y << ", cy=" << cy << "\n";
                z = -distance;
                while (z < distance)
                {
                    cz = __GetChunkPos(z + (int)pos.chunk.z, pos.world.z);
                    std::cout << "z=" << z << ", cz=" << cz << "\n";


                    if (x == -distance || y == -distance || z == -distance ||
                        x + ChunkSize > distance || y + ChunkSize > distance || z + ChunkSize > distance)
                    {
                        CastChunk* r = new CastChunk(BaseChunk::CoordsToId(cx, cy, cz));
                        res.push_back(r);

                        for (zx = 0; zx < (int)ChunkSize; ++zx)
                        {
                            dx = ((int)cx - (int)pos.world.x) * (int)ChunkSize + (zx - (int)pos.chunk.x);
                            dx = GR_ABS(dx);
                            for (zy = 0; zy < (int)ChunkSize; ++zy)
                            {
                                dy = ((int)cy - (int)pos.world.y) * (int)ChunkSize + (zy - (int)pos.chunk.y);
                                dy = GR_ABS(dy);
                                for (zz = 0; zz < (int)ChunkSize; ++zz)
                                {
                                    if ((zx == 0 || zx == 1 || zx == ChunkSize - 2 || zx == ChunkSize - 1) &&
                                        (zy == 0 || zy == 1 || zy == ChunkSize - 2 || zy == ChunkSize - 1) &&
                                        (zz == 0 || zz == 1 || zz == ChunkSize - 2 || zz == ChunkSize - 1))
                                        continue;
                                    dz = ((int)cz - (int)pos.world.z) * (int)ChunkSize + (zz - (int)pos.chunk.z);
                                    dz = GR_ABS(dz);
                                    if (dx <= distance && dy <= distance && dz <= distance)
                                        r->AddCube(zx, zy, zz);
                                }
                            }
                        }
                    }
                    else
                        res.push_back(new CastChunk(BaseChunk::CoordsToId(cx, cy, cz), true));

                    if (z == -distance && __GetCubePos(z + (int)pos.chunk.z) != 0)
                        z += (int)ChunkSize - (int)pos.chunk.z;
                    //{
                    //    while (__GetCubePos(z + pos.chunk.z) != 0)
                    //        z += 1;
                    //}
                    else
                        z += ChunkSize;
                }
                if (y == -distance && __GetCubePos(y + (int)pos.chunk.y) != 0)
                    y += (int)ChunkSize - (int)pos.chunk.y;
                //{
                //    while (__GetCubePos(y + pos.chunk.y) != 0)
                //        y += 1;
                //}
                else
                    y += ChunkSize;
            }
            if (x == -distance && __GetCubePos(x + (int)pos.chunk.x) != 0)
                x += (int)ChunkSize - (int)pos.chunk.x;
            //{
            //    while (__GetCubePos(x + pos.chunk.x) != 0)
            //        x += 1;
            //}
            else
                x += ChunkSize;
        }
        return res;
    }
        /*
        std::vector<Tools::Vector3i> preRes;

        __CubeArea(pos, distance, preRes);

        return __TabToTab(preRes, pos);
    }
    */

    std::vector<CastChunk*> RayCast::SphereArea(Common::Position const& pos, float distance)
    {
        return RayCast::CubeArea(pos, distance);
        //std::vector<Tools::Vector3i> preRes;

        //__SphereArea(pos, distance, preRes);

        //return __TabToTab(preRes, pos);
    }
}
