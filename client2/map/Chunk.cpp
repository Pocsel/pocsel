#include "client2/precompiled.hpp"

#include "client2/map/Chunk.hpp"
#include "tools/IRenderer.hpp"

namespace Client { namespace Map {

        Chunk::Chunk(IdType id)
            : Common::BaseChunk(id)
        {
        }

        Chunk::Chunk(CoordsType const& c)
            : Common::BaseChunk(c)
        {
        }

        bool Chunk::HasTransparentCube() const
        {
            //return this->_dispChunk.HasTransparentCube();
            return false;
        }

        void Chunk::RefreshDisplay(Tools::IRenderer& renderer, Map const& map)
        {
            //this->_dispChunk.Refresh(renderer, map);
        }

        void Chunk::Render(Tools::IRenderer& renderer, Common::Camera const& camera)
        {
            //this->_dispChunk.Render(camera, renderer);
        }

}}
