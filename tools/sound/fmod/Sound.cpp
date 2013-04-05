#include <fmodex/fmod_errors.h>

#include "tools/sound/fmod/Sound.hpp"
#include "tools/sound/fmod/SoundSystem.hpp"
#include "tools/sound/fmod/Channel.hpp"
#include "common/Resource.hpp"

namespace Tools { namespace Sound { namespace Fmod {

    Sound::Sound(SoundSystem const& system, std::string const& path) :
        _system(system)
    {
        if (this->_system._system == 0)
            throw std::runtime_error("Sound::Fmod::Sound: Cannot load \"" + path + "\" because sound system is not initialized.");
        FMOD_RESULT result;
        if ((result = this->_system._system->createSound(path.c_str(), FMOD_3D, nullptr /* create sound ex info ptr */, &this->_sound)) != FMOD_OK)
            throw std::runtime_error("Sound::Fmod::Sound: Cannot load \"" + path + "\": " + FMOD_ErrorString(result));
    }

    Sound::Sound(SoundSystem const& system, Common::Resource const& data) :
        _system(system)
    {
        if (this->_system._system == 0)
            throw std::runtime_error("Sound::Fmod::Sound: Cannot load sound because sound system is not initialized.");
        FMOD_CREATESOUNDEXINFO settings;
        memset(&settings, 0, sizeof(settings));
        settings.cbsize = sizeof(settings);
        settings.length = data.size;
        FMOD_RESULT result;
        if ((result = this->_system._system->createSound(static_cast<char const*>(data.data), FMOD_3D | FMOD_OPENMEMORY, &settings, &this->_sound)) != FMOD_OK)
            throw std::runtime_error("Sound::Fmod::Sound: Cannot load sound: " + std::string(FMOD_ErrorString(result)));
    }

    Sound::Sound(SoundSystem const& system, std::unique_ptr<Common::Resource> data) :
        _system(system),
        _data(std::move(data))
    {
        if (this->_system._system == 0)
            throw std::runtime_error("Sound::Fmod::Sound: Cannot load sound because sound system is not initialized.");
        FMOD_CREATESOUNDEXINFO settings;
        memset(&settings, 0, sizeof(settings));
        settings.cbsize = sizeof(settings);
        settings.length = this->_data->size;
        FMOD_RESULT result;
        if ((result = this->_system._system->createSound(static_cast<char const*>(this->_data->data), FMOD_3D | FMOD_OPENMEMORY | FMOD_CREATESTREAM, &settings, &this->_sound)) != FMOD_OK)
            throw std::runtime_error("Sound::Fmod::Sound: Cannot load sound: " + std::string(FMOD_ErrorString(result)));
    }

    Sound::~Sound()
    {
        FMOD_RESULT result;
        if ((result = this->_sound->release()) != FMOD_OK)
        {
            char nameBuf[512];
            this->_sound->getName(nameBuf, 512);
            std::string name(nameBuf);
            Tools::error << "Sound::Fmod::Sound: Failed to unload sound \"" << name << "\": " << FMOD_ErrorString(result) << std::endl;
        }
    }

    std::unique_ptr<AChannel> Sound::GetChannel() const
    {
        FMOD::Channel* channel;
        this->_system._system->playSound(FMOD_CHANNEL_FREE, this->_sound, true /* paused */, &channel);
        return std::unique_ptr<AChannel>(new Channel(channel));
    }

}}}
