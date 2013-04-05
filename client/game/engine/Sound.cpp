#include "client/game/engine/Sound.hpp"
#include "tools/sound/ISound.hpp"
#include "tools/sound/AChannel.hpp"

namespace Client { namespace Game { namespace Engine {

    Sound::Sound(Uint32 id, Uint32 doodadId, std::shared_ptr<Tools::Sound::ISound> sound) :
        _id(id),
        _doodadId(doodadId),
        _sound(sound)
    {
    }

    Sound::~Sound()
    {
    }

    void Sound::Play() const
    {
        this->_sound->GetChannel()->Play();
    }

}}}
