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
        FMOD_VECTOR _earsPos;
        FMOD_VECTOR _earsVel;
        FMOD_VECTOR _earsForward;
        FMOD_VECTOR _earsUp;

    public:
        SoundSystem();
        ~SoundSystem();
        virtual void Update();
        virtual void SetEars(glm::fvec3 const& pos, glm::fvec3 const& vel, glm::fvec3 const& forward, glm::fvec3 const& up);
        virtual void SetEars(glm::fvec3 const& pos, glm::fvec3 const& vel, glm::fvec3 const& forward);
        virtual void SetEars(glm::fvec3 const& pos, glm::fvec3 const& vel);
        virtual void SetEars(glm::fvec3 const& pos);
        virtual void SetEarsOrientation(glm::fvec3 const& forward, glm::fvec3 const& up);
        virtual ISound* CreateSound(std::string const& path) const; // decompress entire file from disk to memory
        virtual ISound* CreateSound(Common::Resource const& data) const; // decompress entire data to memory (no ownership needed)
        virtual ISound* CreateSound(std::unique_ptr<Common::Resource> data) const; // streaming from compressed & owned data (for big sounds/music)
    };

}}}

#endif
