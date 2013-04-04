#ifndef __TOOLS_SOUND_FMOD_SOUND_HPP__
#define __TOOLS_SOUND_FMOD_SOUND_HPP__

#include <fmodex/fmod.hpp>

#include "tools/sound/ISound.hpp"

namespace Tools { namespace Sound { namespace Fmod {

    class SoundSystem;

    class Sound :
        public ISound
    {
    private:
        SoundSystem const& _system;
        FMOD::Sound* _sound;

    public:
        Sound(SoundSystem const& system, std::string const& path);
        ~Sound();
        virtual void Play() const;
    };

}}}

#endif
