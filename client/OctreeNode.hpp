#ifndef __COMMON_OCTREENODE_HPP__
#define __COMMON_OCTREENODE_HPP__

#include "client/Chunk.hpp"
#include "common/constants.hpp"
#include "common/Camera.hpp"
#include "tools/AlignedBox.hpp"
#include "tools/MemoryPool.hpp"
#include "tools/Vector3.hpp"
#include "tools/IRenderer.hpp"

namespace Client {

    class OctreeNode
    {
        private:
            typedef ::Client::Chunk ChunkType;

            typedef ChunkType::CoordsType CoordsType;
            typedef ChunkType::CoordType CoordType;

            enum
            {
                bottomBackLeft = 0,
                bottomFrontLeft = 1,
                topBackLeft = 2,
                topFrontLeft = 3,
                bottomBackRight = 4,
                bottomFrontRight = 5,
                topBackRight = 6,
                topFrontRight = 7,
            };

            // 'bottomBackLeft':   Node(self, coords=(x, y, z),   size=self._subSize),
            // 'bottomFrontLeft':  Node(self, coords=(x, y, cz),  size=self._subSize),
            // 'topBackLeft':      Node(self, coords=(x, cy, z),   size=self._subSize),
            // 'topFrontLeft':     Node(self, coords=(x, cy, cz),  size=self._subSize),
            // 'bottomBackRight':  Node(self, coords=(cx, y, z),  size=self._subSize),
            // 'bottomFrontRight': Node(self, coords=(cx, y, cz), size=self._subSize),
            // 'topBackRight':     Node(self, coords=(cx, cy, z),  size=self._subSize),
            // 'topFrontRight':    Node(self, coords=(cx, cy, cz), size=self._subSize),

        private:
            Tools::MemoryPool<OctreeNode>& _memoryPool;
            CoordsType _coords;
            CoordType _size;
            ChunkType* _chunk;
            OctreeNode* _parent;
            OctreeNode* _childs[8];
            bool _empty;
            CoordsType _centerCoords;

        public:
            // Root node
            OctreeNode(Tools::MemoryPool<OctreeNode>& memoryPool, CoordsType const& coords, CoordType const& size) :
                _memoryPool(memoryPool),
                _coords(coords),
                _size(size),
                _chunk(0),
                _parent(0),
                _empty(true)
            {
                this->_Init();
            }

            // intermediate node
            OctreeNode(Tools::MemoryPool<OctreeNode>& memoryPool, OctreeNode& parent, CoordsType const& coords, CoordType const& size) :
                _memoryPool(memoryPool),
                _coords(coords),
                _size(size),
                _chunk(0),
                _parent(&parent),
                _empty(true)
            {
                this->_Init();
            }

            // leaf
            OctreeNode(Tools::MemoryPool<OctreeNode>& memoryPool, OctreeNode& parent, ChunkType& chunk) :
                _memoryPool(memoryPool),
                _coords(chunk.coords),
                _size(1),
                _chunk(&chunk),
                _parent(&parent),
                _empty(true)
            {
                this->_Init();
            }

            ~OctreeNode()
            {
                //for (unsigned int i = 0 ; i < 8 ; ++i)
                //    Tools::Delete(this->_childs[i]);
                Tools::Delete(this->_chunk);
            }

            void InsertChunk(ChunkType& chunk)
            {
                assert(this->_Contains(chunk.coords) &&
                        "This chunk is not supposed to be here.");
                if (this->_chunk != 0)
                {
                    assert(false && "faut faire qqc de propre ici");
                    // TODO
                    // effacer un chunk, le remplacer
                }

                this->_empty = false;


                unsigned int index = this->_CoordsToIndex(chunk.coords);

                if (this->_childs[index] == 0)
                {
                    if (this->_size == 2)
                    {
                        assert(this->_childs[index] == 0 && "Ce node devrait être vide");
                        this->_childs[index] = &(this->_memoryPool.CreateElement<Tools::MemoryPool<OctreeNode>&, OctreeNode&, ChunkType&>(this->_memoryPool, *this, chunk));
                    }
                    else
                    {
                        CoordsType coords = this->_CoordsToChildCoords(chunk.coords);
                        this->_childs[index] = &(this->_memoryPool.CreateElement<Tools::MemoryPool<OctreeNode>&, OctreeNode&, CoordsType, CoordType>(this->_memoryPool, *this, coords, this->_size / 2));
                        this->_childs[index]->InsertChunk(chunk);
                    }
                }
                else
                    this->_childs[index]->InsertChunk(chunk);
            }

            void Render(Common::Camera const& camera, Tools::Frustum const& frustum, Tools::IRenderer& renderer, std::multimap<double, ChunkType*>& transparentChunks)
            {
                if (this->_empty == true && this->_chunk == 0)
                    return;

                // pour virer l'octree
//                    this->ForceRender(camera, renderer); return;

                auto tmp = Tools::Vector3d(this->_coords) - Tools::Vector3d(camera.position.world);
                tmp *= Common::ChunkSize;
                tmp -= Tools::Vector3d(camera.position.chunk);
                Tools::AlignedBox box(tmp, tmp + Tools::Vector3d(this->_size*Common::ChunkSize));

                switch (frustum.Contains(box))
                {
                case Tools::AbstractCollider::Inside:
                    this->ForceRender(camera, renderer, transparentChunks);
                    break;

                case Tools::AbstractCollider::Intersecting:
                    if (this->_chunk != 0)
                        this->_RenderChunk(camera, renderer, transparentChunks);
                    else
                        for (unsigned int i = 0 ; i < 8 ; ++i)
                            if (this->_childs[i] != 0)
                                this->_childs[i]->Render(camera, frustum, renderer, transparentChunks);
                    break;
                case Tools::AbstractCollider::Outside: break; //nothing to do
                }
            }

            void ForceRender(Common::Camera const& camera, Tools::IRenderer& renderer, std::multimap<double, ChunkType*>& transparentChunks)
            {
                if (this->_empty == true && this->_chunk == 0)
                    return;
                if (this->_chunk != 0)
                    this->_RenderChunk(camera, renderer, transparentChunks);
                else
                {
                    for (unsigned int i = 0 ; i < 8 ; ++i)
                        if (this->_childs[i] != 0)
                            this->_childs[i]->ForceRender(camera, renderer, transparentChunks);
                }
            }

        private:
            void _Init()
            {
                for (unsigned int i = 0 ; i < 8 ; ++i)
                    this->_childs[i] = 0;
                this->_centerCoords.x = this->_coords.x + this->_size / 2;
                this->_centerCoords.y = this->_coords.y + this->_size / 2;
                this->_centerCoords.z = this->_coords.z + this->_size / 2;
            }

            void _RenderChunk(Common::Camera const& camera, Tools::IRenderer& renderer, std::multimap<double, ChunkType*>& transparentChunks)
            {
                if (this->_chunk->HasTransparentCube())
                {
                    auto const& relativePosition = Common::Position(this->_chunk->coords, Tools::Vector3f(Common::ChunkSize / 2.0f)) - camera.position;
                    auto dist = relativePosition.GetMagnitudeSquared();
                    auto value = std::multimap<double, ChunkType*>::value_type(-dist, this->_chunk);
                    transparentChunks.insert(value);
                }
                else
                    this->_chunk->Render(camera, renderer);
            }

            unsigned int _CoordsToIndex(CoordsType const& coords)
            {
                return
                    (coords.x >= this->_centerCoords.x ? 4 : 0) +
                    (coords.y >= this->_centerCoords.y ? 2 : 0) +
                    (coords.z >= this->_centerCoords.z ? 1 : 0);
            }

            CoordsType _CoordsToChildCoords(CoordsType const& coords)
            {
                assert(this->_size != 1 && "what the what ?");
                CoordsType child_coords;
                child_coords.x = (coords.x >= this->_centerCoords.x ? this->_centerCoords.x : this->_coords.x);
                child_coords.y = (coords.y >= this->_centerCoords.y ? this->_centerCoords.y : this->_coords.y);
                child_coords.z = (coords.z >= this->_centerCoords.z ? this->_centerCoords.z : this->_coords.z);
                return child_coords;
            }

            bool _Contains(CoordsType const& coords)
            {
                return coords.x >= this->_coords.x && coords.x < this->_coords.x + this->_size &&
                    coords.y >= this->_coords.y && coords.y < this->_coords.y + this->_size &&
                    coords.z >= this->_coords.z && coords.z < this->_coords.z + this->_size;
            }
    };

}

#endif
