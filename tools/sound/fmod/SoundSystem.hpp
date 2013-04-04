#ifndef __TOOLS_SOUND_FMOD_SOUNDSYSTEM_HPP__
#define __TOOLS_SOUND_FMOD_SOUNDSYSTEM_HPP__

#include <fmodex/fmod.hpp>

#include "tools/sound/ISoundSystem.hpp"

namespace Tools { namespace Sound { namespace Fmod {

    class Sound;

    class SoundSystem :
        public ISoundSystem
    {
    friend class Sound;

    private:
        FMOD::System* _system;

    public:
        SoundSystem();
        ~SoundSystem();
        virtual void Update() const;
    };

}}}

#endif
