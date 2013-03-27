#include <fmodex/fmod_errors.h>

#include "client/sound/Sound.hpp"
#include "client/sound/SoundSystem.hpp"

namespace Client { namespace Sound {

    Sound::Sound(SoundSystem const& system, std::string const& path)
    {
        if (system._system == 0)
            throw std::runtime_error("Sound::Sound: Cannot load \"" + path + "\" because sound system is not initialized.");
        FMOD_RESULT result;
        if ((result = system._system->createSound(path.c_str(), FMOD_DEFAULT /* mode */, 0 /* create sound ex info ptr */, &this->_sound)) != FMOD_OK)
            throw std::runtime_error("Sound::Sound: Cannot load \"" + path + "\": " + FMOD_ErrorString(result));
    }

    Sound::~Sound()
    {
        FMOD_RESULT result;
        if ((result = this->_sound->release()) != FMOD_OK)
        {
            char nameBuf[512];
            this->_sound->getName(nameBuf, 512);
            std::string name(nameBuf);
            Tools::error << "Sound::Sound: Failed to unload sound \"" << name << "\": " << FMOD_ErrorString(result) << std::endl;
        }
    }

}}
