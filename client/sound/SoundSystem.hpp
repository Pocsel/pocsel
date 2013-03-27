#ifndef __CLIENT_SOUND_SOUNDSYSTEM_HPP__
#define __CLIENT_SOUND_SOUNDSYSTEM_HPP__

#include <fmodex/fmod.hpp>

namespace Client { namespace Sound {

    class Sound;

    class SoundSystem :
        private boost::noncopyable
    {
    friend class Sound;

    private:
        FMOD::System* _system;

    public:
        SoundSystem();
        ~SoundSystem();
        void Play(Sound const& sound) const;
    };

}}

#endif
