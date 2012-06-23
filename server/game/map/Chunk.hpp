#ifndef __SERVER_CHUNK_HPP__
#define __SERVER_CHUNK_HPP__

#include "common/BaseChunk.hpp"

class btBvhTriangleMeshShape;
class btRigidBody;

namespace Server { namespace Game { namespace Map {

    struct Chunk : public Common::BaseChunk
    {
    private:
        btBvhTriangleMeshShape* _shape;
        btRigidBody* _body;

    public:
        explicit Chunk(IdType id);// : BaseChunk(id) {}
        explicit Chunk(CoordsType const& coords);// : BaseChunk(coords) {}
        ~Chunk();

        void InitBody();
        btRigidBody* GetBody() { return this->_body; }
    };

}}}

#endif
