#ifndef __SERVER_MAPGEN_IRAND_HPP__
# define __SERVER_MAPGEN_IRAND_HPP__

namespace Server
{
    namespace MapGen
    {

        class IRand
        {
        public:
            virtual ~IRand() {}
            virtual Int32 Rand() = 0;
            virtual void ReSeed(Int32 seed) = 0;
        };

    }
}

#endif
