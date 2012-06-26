#ifndef __COMMON_PHYSICS_CHUNK_HPP__
#define __COMMON_PHYSICS_CHUNK_HPP__

namespace Common {
    struct BaseChunk;
}

class btCompoundShape;
class btRigidBody;

namespace Common { namespace Physics {

    class Chunk :
        private boost::noncopyable
    {
    private:
        btCompoundShape* _shape;
        btRigidBody* _body;

    public:
        explicit Chunk(Common::BaseChunk const& source);
        ~Chunk();

        btRigidBody* GetBody() { return this->_body; }
    };

}}

#endif

