#include "server/game/map/gen/RandMersenneTwister.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    RandMersenneTwister::RandMersenneTwister(Int32 seed)
    {
        this->_ReSeed(seed);
    }

    RandMersenneTwister::~RandMersenneTwister()
    {
    }

    void RandMersenneTwister::ReSeed(Int32 seed)
    {
        this->_ReSeed(seed);
    }

    Int32 RandMersenneTwister::Rand()
    {
        if (this->_index == 624)
        {
            this->_GenerateNumbers();
            this->_index = 0;
        }

        Int32 y = this->_tab[this->_index];
        y = y ^ ((y) >> 11);
        y = y ^ ((y << 7) & (2636928640u));
        y = y ^ ((y << 15) & (4022730752u));
        y = y ^ ((y >> 18));

        ++this->_index;
        return y;
    }

    void RandMersenneTwister::_GenerateNumbers()
    {
        Int32 y;
        for (Int32 i = 0; i < 624 ; ++i)
        {
            y = (this->_tab[i] & 0x80000000) +
                ((this->_tab[(i + 1) % 624]) & 0x7FFFFFFF);
            this->_tab[i] = this->_tab[(i + 397) % 624] ^ ((y) >> 1);
            if (y % 2)
            {
                this->_tab[i] = this->_tab[i] ^ (2567483615u);
            }
        }
    }

    void RandMersenneTwister::_ReSeed(Int32 seed)
    {
        this->_tab[0] = seed;
        for (Int32 i = 1 ; i < 624 ; ++i)
            this->_tab[i] =
                1812433253 * (this->_tab[i - 1] ^ (this->_tab[i - 1] >> 30)) + i;
        this->_index = 0;
    }

}}}}
