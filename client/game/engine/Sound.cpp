#include "client/game/engine/Sound.hpp"

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

}}}
