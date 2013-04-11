#ifndef __TOOLS_SOUND_FMOD_SOUND_HPP__
#define __TOOLS_SOUND_FMOD_SOUND_HPP__

#include <fmodex/fmod.hpp>

#include "tools/sound/ISound.hpp"

namespace Common {
    class Resource;
}

namespace Tools { namespace Sound { namespace Fmod {

    class SoundSystem;

    class Sound :
        public ISound
    {
    private:
        SoundSystem const& _system;
        FMOD::Sound* _sound;
        std::unique_ptr<Common::Resource> _data;

    public:
        Sound(SoundSystem const& system, std::string const& path); // decompress entire file from disk to memory
        Sound(SoundSystem const& system, Common::Resource const& data); // decompress entire data to memory (no ownership needed)
        Sound(SoundSystem const& system, std::unique_ptr<Common::Resource> data); // streaming from compressed & owned data (for big sounds/music)
        ~Sound();
        virtual std::unique_ptr<AChannel> GetChannel() const;
    };

}}}

#endif
