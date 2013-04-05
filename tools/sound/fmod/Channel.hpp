#ifndef __TOOLS_SOUND_FMOD_CHANNEL_HPP__
#define __TOOLS_SOUND_FMOD_CHANNEL_HPP__

#include <fmodex/fmod.hpp>

#include "tools/sound/AChannel.hpp"

namespace Tools { namespace Sound { namespace Fmod {

    class Channel :
        public AChannel
    {
    private:
        FMOD::Channel* _channel;
        FMOD_VECTOR _pos;
        FMOD_VECTOR _vel;

    public:
        Channel(FMOD::Channel* channel);
        virtual void SetPaused(bool paused);
        virtual void SetPosition(glm::fvec3 const& pos);
        virtual void SetVelocity(glm::fvec3 const& vel);
    };

}}}

#endif
