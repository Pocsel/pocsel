#ifndef __SERVER_GAME_MAP_GEN_IRAND_HPP__
#define __SERVER_GAME_MAP_GEN_IRAND_HPP__

namespace Server { namespace Game { namespace Map { namespace Gen {

    class IRand
    {
    public:
        virtual ~IRand() {}
        virtual Int32 Rand() = 0;
        virtual void ReSeed(Int32 seed) = 0;
    };

}}}}

#endif
