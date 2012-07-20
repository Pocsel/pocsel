#ifndef __COMMON_PHYSICS_CHUNK_HPP__
#define __COMMON_PHYSICS_CHUNK_HPP__

namespace Common {
    struct BaseChunk;
}

namespace Common { namespace Physics {
    class World;
}}

class btCompoundShape;
class btRigidBody;
struct btDefaultMotionState;

namespace Common { namespace Physics {

    class Chunk :
        private boost::noncopyable
    {
    private:
        Common::Physics::World& _world;
        btCompoundShape* _shape;
        btRigidBody* _body;

    public:
        explicit Chunk(Common::Physics::World& world, Common::BaseChunk const& source);
        ~Chunk();

        btRigidBody* GetBody() { return this->_body; }
    };

}}

#endif

