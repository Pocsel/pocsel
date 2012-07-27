#include "tools/precompiled.hpp"

#include "common/physics/Chunk.hpp"
#include "common/physics/World.hpp"
#include "common/BaseChunk.hpp"

#include "bullet/bullet-all.hpp"

// viewdistance 4, minimumarea 4:
// 203175 c'est avec la technique "arbre" a la con
// 24113 avec des trucs qui se chevauchent sans bug
// 26686 sans les trucs qui se chevauchent sans bug

// à tester: le temps

namespace Common { namespace Physics {

    std::unique_ptr<Tools::Stat::Counter> Chunk::_chunkCounter;
    std::unique_ptr<Tools::Stat::Counter> Chunk::_boxCounter;

    Chunk::~Chunk()
    {
        if (this->_shape)
        {
            *_chunkCounter -= 1;
            *_boxCounter -= this->_shape->getNumChildShapes();
        }
        if (this->_body)
        {
            this->_body->setUserPointer((void*)1);
            this->_world.GetBtWorld().removeRigidBody(this->_body);
        }
        Tools::Delete(this->_body);
        Tools::Delete(this->_shape);
    }

    namespace {
        struct _FillCompoundShape
        {
            static inline void _(std::unique_ptr<btBoxShape>* boxShapes, Common::BaseChunk::CubeType const* cubes, btCompoundShape* shape)
            {
                unsigned int fastForward[ChunkSize3];
                for (unsigned int i = 0; i < ChunkSize3; ++i)
                    fastForward[i] = 0;
                btTransform tr;
                tr.setIdentity();

                unsigned int x, y, z, sizeX, sizeY, sizeZ, idxX, idxY, idxZ, ff;

                for (z = 0; z < Common::ChunkSize;)
                {
                    for (y = 0; y < Common::ChunkSize;)
                    {
                        for (x = 0; x < Common::ChunkSize;)
                        {
                            ff = fastForward[x + y * Common::ChunkSize + z * Common::ChunkSize2];
                            if (ff)
                            {
                                x += ff;
                                continue;
                            }
                            if (!cubes[x + y * Common::ChunkSize + z * Common::ChunkSize2])
                            {
                                assert(fastForward[x + y * Common::ChunkSize + z * Common::ChunkSize2] == 0 &&
                                        "J'ai fait de la merde");
                                ++x;
                                continue;
                            }

                            sizeX = _x(fastForward, cubes, x, y, z);
                            sizeY = _y(fastForward, cubes, x, y, z, sizeX);
                            sizeZ = _z(fastForward, cubes, x, y, z, sizeX, sizeY);
                            idxX = sizeX - 1;
                            idxY = sizeY - 1;
                            idxZ = sizeZ - 1;

                            tr.setOrigin(btVector3(x + sizeX * 0.5, y + sizeY * 0.5, z + sizeZ * 0.5));
                            shape->addChildShape(tr, boxShapes[idxX + idxY * ChunkSize + idxZ * ChunkSize2].get());

                            for (idxX = 0; idxX < sizeX; ++idxX)
                                for (idxY = 0; idxY < sizeY; ++idxY)
                                    for (idxZ = 0; idxZ < sizeZ; ++idxZ)
                                    {
                                        fastForward[(x + idxX) + (y + idxY) * Common::ChunkSize + (z + idxZ) * Common::ChunkSize2] = sizeX;
                                    }

                            x += fastForward[x + y * Common::ChunkSize + z * Common::ChunkSize2];
                        }
                        ++y;
                    }
                    ++z;
                }
            }
            static inline unsigned int _x(unsigned int const* fastForward, Common::BaseChunk::CubeType const* cubes, unsigned int x, unsigned int y, unsigned int z)
            {
                unsigned int i = 1;
                while (x + i < ChunkSize && cubes[x + i + y * ChunkSize + z * ChunkSize2] && !fastForward[x + i + y * ChunkSize + z * ChunkSize2])
                    ++i;
                return i;
            }
            static inline unsigned int _y(unsigned int const* fastForward, Common::BaseChunk::CubeType const* cubes, unsigned int x, unsigned int y, unsigned int z, unsigned int sizeX)
            {
                unsigned int i = 1;
                while (y + i < ChunkSize && _checkX(fastForward, cubes, x, y + i, z, sizeX))
                    ++i;
                return i;
            }
            static inline unsigned int _z(unsigned int const* fastForward, Common::BaseChunk::CubeType const* cubes, unsigned int x, unsigned int y, unsigned int z, unsigned int sizeX, unsigned int sizeY)
            {
                unsigned int i = 1;
                while (z + i < ChunkSize && _checkY(fastForward, cubes, x, y, z + i, sizeX, sizeY))
                    ++i;
                return i;
            }
            static inline bool _checkX(unsigned int const* fastForward, Common::BaseChunk::CubeType const* cubes, unsigned int x, unsigned int y, unsigned int z, unsigned int sizeX)
            {
                for (unsigned int i = 0; i < sizeX; ++i)
                    if (cubes[x + i + y * ChunkSize + z * ChunkSize2] == 0 || fastForward[x + i + y * ChunkSize + z * ChunkSize2])
                        return false;
                return true;
            }
            static inline bool _checkY(unsigned int const* fastForward, Common::BaseChunk::CubeType const* cubes, unsigned int x, unsigned int y, unsigned int z, unsigned int sizeX, unsigned int sizeY)
            {
                for (unsigned int i = 0; i < sizeY; ++i)
                    if (_checkX(fastForward, cubes, x, y + i, z, sizeX) == false)
                        return false;
                return true;
            }
        };
    }

    Chunk::Chunk(Common::Physics::World& world, Common::BaseChunk const& source) :
        _world(world),
        _shape(0),
        _body(0)
    {
        if (source.IsEmpty())
            return;

        static bool boxShapesInit = true;
        static std::unique_ptr<btBoxShape> boxShapes[ChunkSize3];
        if (boxShapesInit)
        {
            boxShapesInit = false;
            for (unsigned int x = 0; x < ChunkSize; ++x)
                for (unsigned int y = 0; y < ChunkSize; ++y)
                    for (unsigned int z = 0; z < ChunkSize; ++z)
                    {
                        boxShapes[x + y * ChunkSize + z * ChunkSize2].reset(new btBoxShape(btVector3(btScalar(x + 1) * 0.5, btScalar(y + 1) * 0.5, btScalar(z + 1) * 0.5)));
                    }
        }

        this->_shape = new btCompoundShape(false);

        Common::BaseChunk::CubeType const* cubes = source.GetCubes();

        _FillCompoundShape::_(boxShapes, cubes, this->_shape);

        if (this->_shape->getNumChildShapes() == 0)
        {
            Tools::Delete(this->_shape);
            this->_shape = 0;
            return;
        }

        this->_shape->createAabbTreeFromChildren();

        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(btVector3((source.coords.x) * 32, (source.coords.y) * 32,(source.coords.z) * 32));

        btRigidBody::btRigidBodyConstructionInfo rbInfo(0, 0, this->_shape, btVector3(0, 0, 0));
        rbInfo.m_restitution = 0.5;
        this->_body = new btRigidBody(rbInfo);
        this->_body->setCenterOfMassTransform(tr);
        this->_body->setUserPointer(this);

        this->_body->setCollisionFlags(this->_body->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);

        this->_world.GetBtWorld().addRigidBody(this->_body);

//        Tools::debug << "physics cubes in this chunk: " << this->_shape->getNumChildShapes() << "\n";
        if (!_boxCounter)
        {
            _chunkCounter.reset(new Tools::Stat::Counter("Chunks"));
            _boxCounter.reset(new Tools::Stat::Counter("Chunk physics boxes"));
        }
        *_chunkCounter += 1;
        *_boxCounter += this->_shape->getNumChildShapes();
    }

}}
