#ifndef __CLIENT_MAP_CHUNK_HPP__
#define __CLIENT_MAP_CHUNK_HPP__

#include "common/BaseChunk.hpp"

namespace Common {
    struct Camera;
}

namespace Tools {
    class IRenderer;
}

namespace Client { namespace Map {
    class Map;
}}

namespace Client { namespace Map {

    class Chunk
        : public Common::BaseChunk,
        private boost::noncopyable
    {
        private:

        public:
            Chunk(IdType id);
            Chunk(CoordsType const& c);

            bool HasTransparentCube() const;
            void RefreshDisplay(Tools::IRenderer& renderer, Map const& map);
            void Render(Tools::IRenderer& renderer, Common::Camera const& camera);
    };

}}

#endif
