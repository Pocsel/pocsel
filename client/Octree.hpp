#ifndef __COMMON_OCTREE_HPP__
#define __COMMON_OCTREE_HPP__

#include <map>

#include "client/Chunk.hpp"
#include "client/OctreeNode.hpp"
#include "tools/MemoryPool.hpp"

namespace Tools {
    class IRenderer;
}

namespace Common {
    struct Camera;
}

namespace Client {

    class Octree
    {
        private:
            typedef ::Client::Chunk ChunkType;
            typedef ChunkType::CoordsType CoordsType;
            typedef ChunkType::CoordType CoordType;

        private:
            Tools::MemoryPool<OctreeNode> _memoryPool;
            OctreeNode* _roots[16];

        public:
            Octree()
            {
                CoordsType coords;
                coords.y = 0;
                for (unsigned int x = 0 ; x < 4 ; ++x)
                {
                    coords.x = x << (22 - 2);
                    for (unsigned int z = 0 ; z < 4 ; ++z)
                    {
                        coords.z = z << (22 - 2);
                        this->_roots[x + z * 4] = &(_memoryPool.CreateElement<Tools::MemoryPool<OctreeNode>&, CoordsType&, int>(_memoryPool, coords, 1 << 20));
                    }
                }
            }

            ~Octree()
            {
                //for (unsigned int i = 0 ; i < 16 ; ++i)
                //    Tools::Delete(this->_roots[i]);
            }

            void InsertChunk(ChunkType& chunk)
            {
                this->_roots[this->_CoordsToRootIndex(chunk.coords)]->InsertChunk(chunk);
            }

            void Dump();
            void Render(Common::Camera const& camera, Tools::IRenderer& renderer)
            {
                Tools::Frustum frustum = camera.GetFrustum();
                ChunkType::Bind();

                std::multimap<double, ChunkType*> transparentChunk;
                for (unsigned int i = 0 ; i < 16 ; ++i)
                    this->_roots[i]->Render(camera, frustum, renderer, transparentChunk);
                for (auto it = transparentChunk.begin(), ite = transparentChunk.end(); it != ite; ++it)
                    it->second->Render(camera, renderer);
                ChunkType::UnBind();
            }

        private:
            unsigned int _CoordsToRootIndex(CoordsType const& coords)
            {
                unsigned int x = (coords.x) >> (22 - 2);
                unsigned int z = (coords.z) >> (22 - 2);
                assert(x < 4 && z < 4 && "Coords hors limite");
                return x + z * 4;
            }
    };

}

#endif
