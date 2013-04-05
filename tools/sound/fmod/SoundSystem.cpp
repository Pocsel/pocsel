#include <fmodex/fmod_errors.h>

#include "tools/sound/fmod/SoundSystem.hpp"
#include "tools/sound/fmod/Sound.hpp"

namespace Tools { namespace Sound { namespace Fmod {

    SoundSystem::SoundSystem()
    {
        FMOD_RESULT result;
        if ((result = FMOD::System_Create(&this->_system)) != FMOD_OK)
        {
            this->_system = nullptr;
            Tools::error << "Failed to create FMOD sound system: " << FMOD_ErrorString(result) << std::endl;
            return;
        }
        if ((result = this->_system->init(32 /* max channels */, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED /* init flags */, nullptr /* extra driver data ptr */)) != FMOD_OK)
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

    void SoundSystem::Update()
    {
        this->_system->update();
    }

    void SoundSystem::SetEars(glm::fvec3 const& pos, glm::fvec3 const& vel, glm::fvec3 const& forward, glm::fvec3 const& up)
    {
        this->_earsPos.x = pos.x;
        this->_earsPos.y = pos.y;
        this->_earsPos.z = pos.z;
        this->_earsVel.x = vel.x;
        this->_earsVel.y = vel.y;
        this->_earsVel.z = vel.z;
        this->_earsForward.x = forward.x;
        this->_earsForward.y = forward.y;
        this->_earsForward.z = forward.z;
        this->_earsUp.x = up.x;
        this->_earsUp.y = up.y;
        this->_earsUp.z = up.z;
        this->_system->set3DListenerAttributes(0 /* listener id */, &this->_earsPos, &this->_earsVel, &this->_earsForward, &this->_earsUp);
    }

    void SoundSystem::SetEars(glm::fvec3 const& pos, glm::fvec3 const& vel, glm::fvec3 const& forward)
    {
        this->_earsPos.x = pos.x;
        this->_earsPos.y = pos.y;
        this->_earsPos.z = pos.z;
        this->_earsVel.x = vel.x;
        this->_earsVel.y = vel.y;
        this->_earsVel.z = vel.z;
        this->_earsForward.x = forward.x;
        this->_earsForward.y = forward.y;
        this->_earsForward.z = forward.z;
        this->_system->set3DListenerAttributes(0 /* listener id */, &this->_earsPos, &this->_earsVel, &this->_earsForward, nullptr);
    }

    void SoundSystem::SetEars(glm::fvec3 const& pos, glm::fvec3 const& vel)
    {
        this->_earsPos.x = pos.x;
        this->_earsPos.y = pos.y;
        this->_earsPos.z = pos.z;
        this->_earsVel.x = vel.x;
        this->_earsVel.y = vel.y;
        this->_earsVel.z = vel.z;
        this->_system->set3DListenerAttributes(0 /* listener id */, &this->_earsPos, &this->_earsVel, nullptr, nullptr);
    }

    void SoundSystem::SetEars(glm::fvec3 const& pos)
    {
        this->_earsPos.x = pos.x;
        this->_earsPos.y = pos.y;
        this->_earsPos.z = pos.z;
        this->_system->set3DListenerAttributes(0 /* listener id */, &this->_earsPos, nullptr, nullptr, nullptr);
    }

}}}
