#ifndef __CLIENT_CHUNK_HPP__
#define __CLIENT_CHUNK_HPP__

#include "client/gui/primitives/Chunk.hpp"
#include "common/BaseChunk.hpp"

namespace Common {
    struct Camera;
}

namespace Tools {
    class IRenderer;
}

namespace Client {

    class Chunk : public Common::BaseChunk
    {
        private:
            Gui::Primitives::Chunk _dispChunk;

            Chunk(Chunk const&);

        public:
            Chunk(IdType id) : Common::BaseChunk(id), _dispChunk(*this) {}
            Chunk(CoordsType const& c) : Common::BaseChunk(c), _dispChunk(*this) {}

            bool HasTransparentCube() const { return this->_dispChunk.HasTransparentCube(); }
            void RefreshDisp(Tools::IRenderer& renderer, World& world) { this->_dispChunk.Refresh(renderer, world); }
            void Render(Common::Camera const& camera, Tools::IRenderer& renderer) {this->_dispChunk.Render(camera, renderer);}

            static void Bind() {Gui::Primitives::Chunk::Bind();}
            static void UnBind() {Gui::Primitives::Chunk::UnBind();}
    };

}

#endif
