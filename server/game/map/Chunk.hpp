#ifndef __SERVER_GAME_MAP_CHUNK_HPP__
#define __SERVER_GAME_MAP_CHUNK_HPP__

#include "common/BaseChunk.hpp"

namespace Common { namespace Physics {
    class Chunk;
}}

class btRigidBody;

namespace Server { namespace Game { namespace Map {

    struct Chunk :
        public Common::BaseChunk,
        private boost::noncopyable
    {
    private:
        Common::Physics::Chunk* _physicsChunk;

    public:
        explicit Chunk(IdType id);// : BaseChunk(id) {}
        explicit Chunk(CoordsType const& coords);// : BaseChunk(coords) {}
        ~Chunk();

        void InitBody();
        btRigidBody* GetBody();
    };

}}}

#endif
