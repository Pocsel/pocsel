#include "common/physics/Chunk.hpp"
#include "common/BaseChunk.hpp"

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    Chunk::~Chunk()
    {
        Tools::Delete(this->_body);
        Tools::Delete(this->_shape);
    }

    namespace {
        template<int Tpow>//, int TorigX, int TorigY, int TorigZ>
            struct megashit
            {
                enum {
                    Tmax = 1 << Tpow,
                    TpowLess = Tpow - 1,
                    TmaxLess = 1 << TpowLess
                };
                static inline bool _(btBoxShape** boxShapes, Common::BaseChunk::CubeType const* cubes, btCompoundShape* shape, int TorigX, int TorigY, int TorigZ)
                {
                    bool full000 = megashit<TpowLess>::_(boxShapes, cubes, shape, TorigX + 0 * TmaxLess, TorigY + 0 * TmaxLess, TorigZ + 0 * TmaxLess);
                    bool full001 = megashit<TpowLess>::_(boxShapes, cubes, shape, TorigX + 0 * TmaxLess, TorigY + 0 * TmaxLess, TorigZ + 1 * TmaxLess);
                    bool full010 = megashit<TpowLess>::_(boxShapes, cubes, shape, TorigX + 0 * TmaxLess, TorigY + 1 * TmaxLess, TorigZ + 0 * TmaxLess);
                    bool full011 = megashit<TpowLess>::_(boxShapes, cubes, shape, TorigX + 0 * TmaxLess, TorigY + 1 * TmaxLess, TorigZ + 1 * TmaxLess);
                    bool full100 = megashit<TpowLess>::_(boxShapes, cubes, shape, TorigX + 1 * TmaxLess, TorigY + 0 * TmaxLess, TorigZ + 0 * TmaxLess);
                    bool full101 = megashit<TpowLess>::_(boxShapes, cubes, shape, TorigX + 1 * TmaxLess, TorigY + 0 * TmaxLess, TorigZ + 1 * TmaxLess);
                    bool full110 = megashit<TpowLess>::_(boxShapes, cubes, shape, TorigX + 1 * TmaxLess, TorigY + 1 * TmaxLess, TorigZ + 0 * TmaxLess);
                    bool full111 = megashit<TpowLess>::_(boxShapes, cubes, shape, TorigX + 1 * TmaxLess, TorigY + 1 * TmaxLess, TorigZ + 1 * TmaxLess);

                    if (
                            full000 &&
                            full001 &&
                            full010 &&
                            full011 &&
                            full100 &&
                            full101 &&
                            full110 &&
                            full111
                            )

                        return true;

                    btTransform tr;
                    tr.setIdentity();

                    if (full000)
                    {
                        tr.setOrigin(btVector3(TorigX + 0 * TmaxLess + 0.5 * TmaxLess, TorigY + 0 * TmaxLess + 0.5 * TmaxLess, TorigZ + 0 * TmaxLess + 0.5 * TmaxLess));
                        shape->addChildShape(tr, boxShapes[TpowLess]);
                    }
                    if (full001)
                    {
                        tr.setOrigin(btVector3(TorigX + 0 * TmaxLess + 0.5 * TmaxLess, TorigY + 0 * TmaxLess + 0.5 * TmaxLess, TorigZ + 1 * TmaxLess + 0.5 * TmaxLess));
                        shape->addChildShape(tr, boxShapes[TpowLess]);
                    }
                    if (full010)
                    {
                        tr.setOrigin(btVector3(TorigX + 0 * TmaxLess + 0.5 * TmaxLess, TorigY + 1 * TmaxLess + 0.5 * TmaxLess, TorigZ + 0 * TmaxLess + 0.5 * TmaxLess));
                        shape->addChildShape(tr, boxShapes[TpowLess]);
                    }
                    if (full011)
                    {
                        tr.setOrigin(btVector3(TorigX + 0 * TmaxLess + 0.5 * TmaxLess, TorigY + 1 * TmaxLess + 0.5 * TmaxLess, TorigZ + 1 * TmaxLess + 0.5 * TmaxLess));
                        shape->addChildShape(tr, boxShapes[TpowLess]);
                    }
                    if (full100)
                    {
                        tr.setOrigin(btVector3(TorigX + 1 * TmaxLess + 0.5 * TmaxLess, TorigY + 0 * TmaxLess + 0.5 * TmaxLess, TorigZ + 0 * TmaxLess + 0.5 * TmaxLess));
                        shape->addChildShape(tr, boxShapes[TpowLess]);
                    }
                    if (full101)
                    {
                        tr.setOrigin(btVector3(TorigX + 1 * TmaxLess + 0.5 * TmaxLess, TorigY + 0 * TmaxLess + 0.5 * TmaxLess, TorigZ + 1 * TmaxLess + 0.5 * TmaxLess));
                        shape->addChildShape(tr, boxShapes[TpowLess]);
                    }
                    if (full110)
                    {
                        tr.setOrigin(btVector3(TorigX + 1 * TmaxLess + 0.5 * TmaxLess, TorigY + 1 * TmaxLess + 0.5 * TmaxLess, TorigZ + 0 * TmaxLess + 0.5 * TmaxLess));
                        shape->addChildShape(tr, boxShapes[TpowLess]);
                    }
                    if (full111)
                    {
                        tr.setOrigin(btVector3(TorigX + 1 * TmaxLess + 0.5 * TmaxLess, TorigY + 1 * TmaxLess + 0.5 * TmaxLess, TorigZ + 1 * TmaxLess + 0.5 * TmaxLess));
                        shape->addChildShape(tr, boxShapes[TpowLess]);
                    }

                    //for (int x = TorigX; x < Tmax; ++x)
                    //{
                    //    for (int y = TorigY; y < Tmax; ++y)
                    //    {
                    //        for (int z = TorigZ; z < Tmax; ++z)
                    //        {
                    //        }
                    //    }
                    //}

                    return false;
                }
            };
        template<>//int TorigX, int TorigY, int TorigZ>
            struct megashit<0>//, TorigX, TorigY, TorigZ>
            {
                static inline bool _(btBoxShape** boxShapes, Common::BaseChunk::CubeType const* cubes, btCompoundShape* shape, int origX, int origY, int origZ)
                {
                    return cubes[origX + origY * Common::ChunkSize + origZ * Common::ChunkSize2] != 0;
                    //return false;
                }
            };
    }

    Chunk::Chunk(Common::BaseChunk const& source) :
        _shape(0),
        _body(0)
    {
        if (source.IsEmpty())
            return;

        static btBoxShape** boxShapes = 0;
        if (boxShapes == 0)
        {
            boxShapes = new btBoxShape*[6];
            boxShapes[0] = new btBoxShape(btVector3(0.5, 0.5, 0.5));
            boxShapes[1] = new btBoxShape(btVector3(1, 1, 1));
            boxShapes[2] = new btBoxShape(btVector3(2, 2, 2));
            boxShapes[3] = new btBoxShape(btVector3(4, 4, 4));
            boxShapes[4] = new btBoxShape(btVector3(8, 8, 8));
            boxShapes[5] = new btBoxShape(btVector3(16, 16, 16));
        }

        this->_shape = new btCompoundShape(false);

        std::vector<int> colCubes(Common::ChunkSize3);
        std::memset(colCubes.data(), 0, Common::ChunkSize3);
        int number = 0;

        Common::BaseChunk::CubeType const* cubes = source.GetCubes();

        if (megashit<5>/*, 0, 0, 0>*/::_(boxShapes, cubes, this->_shape, 0, 0, 0))
        {
            btTransform tr;
            tr.setIdentity();
            tr.setOrigin(btVector3(16, 16, 16));
            this->_shape->addChildShape(tr, boxShapes[5]);
        }

        /*
        unsigned int x, y, z, cubeOffset;
        for (z = 0; z < Common::ChunkSize; ++z)
        {
            for (y = 0; y < Common::ChunkSize; ++y)
            {
                for (x = 0; x < Common::ChunkSize; ++x)
                {
                    cubeOffset = x + y * Common::ChunkSize + z * Common::ChunkSize2;
                    if (cubes[cubeOffset] == 0)
                        continue;

                    if (x == Common::ChunkSize - 1 || !cubes[cubeOffset + 1] ||
                            y == Common::ChunkSize - 1 || !cubes[cubeOffset + Common::ChunkSize] ||
                            z == Common::ChunkSize - 1 || !cubes[cubeOffset + Common::ChunkSize2] ||
                            x == 0 || !cubes[cubeOffset - 1] ||
                            y == 0 || !cubes[cubeOffset - Common::ChunkSize] ||
                            z == 0 || !cubes[cubeOffset - Common::ChunkSize2])
                    {
                        colCubes[cubeOffset] = 1;
                        ++number;
                    }
                    //{
                    //    btTransform tr;
                    //    tr.setIdentity();
                    //    tr.setOrigin(btVector3((double)x + 0.5, (double)y + 0.5, (double)z + 0.5));
                    //    this->_shape->addChildShape(tr, boxShape);
                    //}
                }
            }
        }

        std::cout << "number: " << number << "\n";
        //std::cout << "number: " << this->_shape->getNumChildShapes() << "\n";

        if (number == 5768)
        {
            btTransform tr;
            tr.setIdentity();
            tr.setOrigin(btVector3(16, 16, 16));
            this->_shape->addChildShape(tr, megaBoxShape);
        }
        else
        for (z = 0; z < Common::ChunkSize; ++z)
        {
            for (y = 0; y < Common::ChunkSize; ++y)
            {
                for (x = 0; x < Common::ChunkSize; ++x)
                {
                    cubeOffset = x + y * Common::ChunkSize + z * Common::ChunkSize2;
                    if (cubes[cubeOffset] == 0)
                        continue;

                    if (colCubes[cubeOffset])
                    {
                        btTransform tr;
                        tr.setIdentity();
                        tr.setOrigin(btVector3((double)x + 0.5, (double)y + 0.5, (double)z + 0.5));
                        this->_shape->addChildShape(tr, boxShape);
                    }
                }
            }
        }
        */
        this->_shape->createAabbTreeFromChildren();

        std::cout << "number: " << this->_shape->getNumChildShapes() << "\n";
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3((source.coords.x) * 32, (source.coords.y) * 32,(source.coords.z) * 32));

        btDefaultMotionState* myMotionState = new btDefaultMotionState(tr);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, this->_shape, btVector3(0, 0, 0));
        this->_body = new btRigidBody(rbInfo);

    }

}}
