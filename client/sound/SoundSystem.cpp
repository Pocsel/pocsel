#include <fmodex/fmod_errors.h>

#include "client/sound/SoundSystem.hpp"
#include "client/sound/Sound.hpp"

namespace Client { namespace Sound {

    SoundSystem::SoundSystem()
    {
        FMOD_RESULT result;
        if ((result = FMOD::System_Create(&this->_system)) != FMOD_OK)
        {
            this->_system = nullptr;
            Tools::error << "Failed to create FMOD sound system: " << FMOD_ErrorString(result) << std::endl;
            return;
        }
        if ((result = this->_system->init(32 /* max channels */, FMOD_INIT_NORMAL /* init flags */, nullptr /* extra driver data ptr */)) != FMOD_OK)
        {
            this->_system = nullptr;
            Tools::error << "Failed to initialize FMOD sound system: " << FMOD_ErrorString(result) << std::endl;
            return;
        }
        Tools::log << "FMOD sound system successfully initialized." << std::endl;
    }

    SoundSystem::~SoundSystem()
    {
        if (this->_system == nullptr)
            return;
        FMOD_RESULT result;
        if ((result = this->_system->release()) != FMOD_OK)
            Tools::error << "Failed to release FMOD sound system: " << FMOD_ErrorString(result) << std::endl;
        else
            Tools::log << "FMOD sound system successfully released." << std::endl;
    }

    void SoundSystem::Play(Sound const& sound) const
    {
        this->_system->playSound(FMOD_CHANNEL_FREE /* channel id */, sound._sound, false /* paused */, nullptr /* channel ptr */);
    }

}}
