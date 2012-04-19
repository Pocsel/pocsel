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
                   std::map<double, glm::ivec3>& res)
        {
            double x, y, z;
            const double
                vx = pos.direction.x,
                vy = pos.direction.y,
                vz = pos.direction.z;
            Position p = GetPositionInChunk(pos.position);
            const double
                px = p.x,
                pz = p.z,
                py = p.y;

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

                    res[dist] = glm::ivec3((int)GR_NEGFLOOR(x) - (sign < 0 ? 1 : 0),
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

                    res[dist] = glm::ivec3((int)GR_NEGFLOOR(x),
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

                    res[dist] = glm::ivec3((int)GR_NEGFLOOR(x),
                                                (int)GR_NEGFLOOR(y),
                                                (int)GR_NEGFLOOR(z) - (sign < 0 ? 1 : 0)
                                               );

                    z += sign;
                }
            }
        }

        template<typename T>
        std::vector<Common::CubePosition> __MapToTab(std::map<T, glm::ivec3> const& preRes, Common::Position const& pos)
        {
            Common::BaseChunk::CoordsType world;
            glm::uvec3 chunk;

            std::vector<Common::CubePosition> res(preRes.size());

            for (auto mit = preRes.begin(), mite = preRes.end(); mit != mite; ++mit)
            {
                world = GetChunkCoords(pos);

                glm::ivec3 r = mit->second;

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

                res.push_back(Common::CubePosition(world * Common::ChunkSize + chunk));
            }

            return res;
        }

    }

    std::vector<Common::CubePosition> RayCast::Ray(Common::OrientedPosition const& pos, float distance)
    {
        std::map<double, glm::ivec3> preRes;

        __Ray(pos, distance, preRes);


        return __MapToTab(preRes, pos.position);
    }

    namespace {

        inline bool __PointInSphere(Position const& pointPos, Position const& spherePos, float const radius)
        {
            double distX = spherePos.x - pointPos.x;
            double distY = spherePos.y - pointPos.y;
            double distZ = spherePos.z - pointPos.z;

            double sqDist = (distX * distX) + (distY * distY) + (distZ * distZ);

            return sqDist < radius * radius;
        }

        inline double __Clamp(double x, double min, double max)
        {
            return x < min ? min : (x > max ? max : x);
        }

        inline double __Farest(double x, double min, double max)
        {
            return x < min ? max : (x > max ? min : (std::abs(x - min) < std::abs(x - max) ? max : min));
        }

        // 0 si exterieur
        // 1 si a moitie dedans
        // 2 si dedans
        inline int __ChunkInSphere(RayCast::BigChunk::CoordsType const& chunkCoords, Position const& spherePos, float const radius)
        {
            Position chunkPos(chunkCoords * ChunkSize);

            double clampedX = __Clamp(spherePos.x, chunkPos.x + 0.5, chunkPos.x - 0.5 + ChunkSize);
            double clampedY = __Clamp(spherePos.y, chunkPos.y + 0.5, chunkPos.y - 0.5 + ChunkSize);
            double clampedZ = __Clamp(spherePos.z, chunkPos.z + 0.5, chunkPos.z - 0.5 + ChunkSize);

            double distX = spherePos.x - clampedX;
            double distY = spherePos.y - clampedY;
            double distZ = spherePos.z - clampedZ;

            double sqDist = (distX * distX) + (distY * distY) + (distZ * distZ);

            if (sqDist > radius * radius)
                return 0;

            double farX = __Farest(spherePos.x, chunkPos.x + 0.5, chunkPos.x - 0.5 + ChunkSize);
            double farY = __Farest(spherePos.y, chunkPos.y + 0.5, chunkPos.y - 0.5 + ChunkSize);
            double farZ = __Farest(spherePos.z, chunkPos.z + 0.5, chunkPos.z - 0.5 + ChunkSize);

            distX = spherePos.x - farX;
            distY = spherePos.y - farY;
            distZ = spherePos.z - farZ;

            sqDist = (distX * distX) + (distY * distY) + (distZ * distZ);

            if (sqDist > radius * radius)
                return 1;
            return 2;
        }

        inline unsigned int __GetCubePos(int i)
        {
            while (i < 0)
                i += ChunkSize;
            i %= ChunkSize;
            return i;
        }
    }

    std::vector<CastChunk*> RayCast::SphereArea(Common::Position const& pos, float distance)
    {
        std::unordered_map<BigChunk::IdType, std::vector<CastChunk*>> preRes;
        unsigned int count = 0;

        int cx, cy, cz;
        int x, y, z;

        for (x = -distance; x < distance;)
        {
            cx = (int)((x + pos.x) / (int)Common::ChunkSize);

            for (y = -distance; y < distance;)
            {
                cy = (int)((y + pos.y) / (int)Common::ChunkSize);

                for (z = -distance; z < distance;)
                {
                    cz = (int)((z + pos.z) / (int)Common::ChunkSize);

                    int chunkPos = __ChunkInSphere(BigChunk::CoordsType(cx, cy, cz), pos, distance);
                    if (chunkPos == 1)
                    {
                        BaseChunk::IdType cId = BaseChunk::CoordsToId(cx, cy, cz);
                        preRes[BigChunk::GetId(cId)].push_back(new CastChunk(cId, CastChunk::CastType::Sphere, pos, distance));
                        ++count;
                    }
                    else if (chunkPos == 2)
                    {
                        BaseChunk::IdType cId = BaseChunk::CoordsToId(cx, cy, cz);
                        preRes[BigChunk::GetId(cId)].push_back(new CastChunk(cId));
                        ++count;
                    }

                    if (z == (int)-distance)
                        z += (int)ChunkSize - (int)__GetCubePos(z + (int)GetCubeCoordsInChunk(pos).z);
                    else
                        z += ChunkSize;
                }
                if (y == (int)-distance)
                    y += (int)ChunkSize - (int)__GetCubePos(y + (int)GetCubeCoordsInChunk(pos).y);
                else
                    y += ChunkSize;
            }
            if (x == (int)-distance)
                x += (int)ChunkSize - (int)__GetCubePos(x + (int)GetCubeCoordsInChunk(pos).x);
            else
                x += ChunkSize;
        }

        std::vector<CastChunk*> res;
        res.reserve(count);
        for (auto it = preRes.begin(), ite = preRes.end(); it != ite; ++it)
        {
            res.insert(res.end(), it->second.begin(), it->second.end());
        }

        Tools::debug << "Explosion: " << res.size() << " chunks\n";
        return res;
    }

    std::vector<BaseChunk::CoordsType> CastChunk::_GetCubesSphereCast()
    {
        BaseChunk::CoordsType coords(BaseChunk::IdToCoords(this->id));
        int cx = coords.x;
        int cy = coords.y;
        int cz = coords.z;
        int zx, zy, zz;
        Position const& pos = this->_origin;
        float distance = this->_distance;

        std::vector<BaseChunk::CoordsType> res;
        res.reserve(ChunkSize3);

        for (zx = 0; zx < (int)ChunkSize; ++zx)
        {
            for (zy = 0; zy < (int)ChunkSize; ++zy)
            {
                for (zz = 0; zz < (int)ChunkSize; ++zz)
                {
                    //if (((zx == 0 || zx == 1 || zx == ChunkSize - 2 || zx == ChunkSize - 1) &&
                    //            (zy == 0 || zy == 1 || zy == ChunkSize - 2 || zy == ChunkSize - 1) &&
                    //            (zz == 0 || zz == 1 || zz == ChunkSize - 2 || zz == ChunkSize - 1)) ||
                    //        (zx == zy && zy == zz) ||
                    //        (zx == (int)ChunkSize-1-zy && (int)ChunkSize-1-zy == zz) ||
                    //        ((int)ChunkSize-1-zx == zy && zy == zz) ||
                    //        (zx == zy && zy == (int)ChunkSize-1-zz) ||

                    //        ((zx == 0 || zx == ChunkSize - 1) && (zz == 0 || zz == ChunkSize - 1)) ||
                    //        ((zx == 0 || zx == ChunkSize - 1) && (zy == 0 || zy == ChunkSize - 1)) ||
                    //        ((zy == 0 || zy == ChunkSize - 1) && (zz == 0 || zz == ChunkSize - 1))
                    //   )
                    //    continue;

                    if (__PointInSphere(Position(cx * ChunkSize + zx, cy * ChunkSize + zy, cz * ChunkSize + zz), pos, distance))
                        res.push_back(BaseChunk::CoordsType(zx, zy, zz));
                }
            }
        }
        return res;
    }

    std::vector<CastChunk*> RayCast::CubeArea(Common::Position const& pos, float distance)
    {
        std::unordered_map<BigChunk::IdType, std::vector<CastChunk*>> preRes;
        unsigned int count = 0;

        int cx, cy, cz;
        int x, y, z;

        for (x = -distance; x < distance;)
        {
            cx = (int)((x + pos.x) / (int)Common::ChunkSize);

            for (y = -distance; y < distance;)
            {
                cy = (int)((y + pos.y) / (int)Common::ChunkSize);

                for (z = -distance; z < distance;)
                {
                    cz = (int)((z + pos.z) / (int)Common::ChunkSize);

                    BaseChunk::IdType cId = BaseChunk::CoordsToId(cx, cy, cz);

                    if (x == -distance || y == -distance || z == -distance ||
                        x + ChunkSize > distance || y + ChunkSize > distance || z + ChunkSize > distance)
                        preRes[BigChunk::GetId(cId)].push_back(new CastChunk(cId, CastChunk::CastType::Cube, pos, distance));
                    else
                        preRes[BigChunk::GetId(cId)].push_back(new CastChunk(cId));
                    ++count;

                    if (z == (int)-distance)
                        z += (int)ChunkSize - (int)__GetCubePos((int)z + (int)GetCubeCoordsInChunk(pos).z);
                    else
                        z += ChunkSize;
                }
                if (y == (int)-distance)
                    y += (int)ChunkSize - (int)__GetCubePos((int)y + (int)GetCubeCoordsInChunk(pos).y);
                else
                    y += ChunkSize;
            }
            if (x == (int)-distance)
                x += (int)ChunkSize - (int)__GetCubePos((int)x + (int)GetCubeCoordsInChunk(pos).x);
            else
                x += ChunkSize;
        }

        std::vector<CastChunk*> res;
        res.reserve(count);
        for (auto it = preRes.begin(), ite = preRes.end(); it != ite; ++it)
        {
            res.insert(res.end(), it->second.begin(), it->second.end());
        }

        Tools::debug << "Explosion: " << res.size() << " chunks\n";
        return res;
    }

    std::vector<BaseChunk::CoordsType> CastChunk::_GetCubesCubeCast()
    {
        BaseChunk::CoordsType coords(BaseChunk::IdToCoords(this->id));
        int cx = coords.x;
        int cy = coords.y;
        int cz = coords.z;
        int zx, zy, zz, dx, dy, dz;
        Position const& pos = this->_origin;
        float distance = this->_distance;

        std::vector<BaseChunk::CoordsType> res;
        res.reserve(ChunkSize3);

        for (zx = 0; zx < (int)ChunkSize; ++zx)
        {
            dx = (int)std::abs((cx * (int)ChunkSize + zx) - pos.x);
            for (zy = 0; zy < (int)ChunkSize; ++zy)
            {
                dy = (int)std::abs((cy * (int)ChunkSize + zy) - pos.y);
                for (zz = 0; zz < (int)ChunkSize; ++zz)
                {
                    //if (((zx == 0 || zx == 1 || zx == ChunkSize - 2 || zx == ChunkSize - 1) &&
                    //            (zy == 0 || zy == 1 || zy == ChunkSize - 2 || zy == ChunkSize - 1) &&
                    //            (zz == 0 || zz == 1 || zz == ChunkSize - 2 || zz == ChunkSize - 1)) ||
                    //        (zx == zy && zy == zz) ||
                    //        (zx == (int)ChunkSize-1-zy && (int)ChunkSize-1-zy == zz) ||
                    //        ((int)ChunkSize-1-zx == zy && zy == zz) ||
                    //        (zx == zy && zy == (int)ChunkSize-1-zz) ||

                    //        ((zx == 0 || zx == ChunkSize - 1) && (zz == 0 || zz == ChunkSize - 1)) ||
                    //        ((zx == 0 || zx == ChunkSize - 1) && (zy == 0 || zy == ChunkSize - 1)) ||
                    //        ((zy == 0 || zy == ChunkSize - 1) && (zz == 0 || zz == ChunkSize - 1))
                    //   )
                    //    continue;
                    dz = (int)std::abs((cz * (int)ChunkSize + zz) - pos.z);
                    if (dx <= distance && dy <= distance && dz <= distance)
                        res.push_back(BaseChunk::CoordsType(zx, zy, zz));
                }
            }
        }
        return res;
    }

}
