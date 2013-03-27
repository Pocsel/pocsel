#ifndef __CLIENT_SOUND_SOUND_HPP__
#define __CLIENT_SOUND_SOUND_HPP__

#include <fmodex/fmod.hpp>

namespace Client { namespace Sound {

    class SoundSystem;

    class Sound :
        private boost::noncopyable
    {
    friend class SoundSystem;

    private:
        FMOD::Sound* _sound;

    public:
        Sound(SoundSystem const& system, std::string const& path);
        ~Sound();
    };

}}

#endif
