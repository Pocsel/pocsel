#include "client/game/engine/Sound.hpp"
#include "tools/sound/ISound.hpp"
#include "tools/sound/AChannel.hpp"
#include "common/Camera.hpp"

namespace Client { namespace Game { namespace Engine {

    Sound::Sound(Uint32 id, Uint32 doodadId, std::shared_ptr<Tools::Sound::ISound> sound, Common::Position const& pos) :
        _id(id),
        _doodadId(doodadId),
        _sound(sound),
        _channel(sound->GetChannel()),
        _pos(pos)
    {
    }

    Sound::~Sound()
    {
    }

    void Sound::Play() const
    {
        this->_channel->Play();
    }

    void Sound::SetPosition(glm::fvec3 const& pos, Common::Camera const& cam)
    {
        this->_pos = pos;
        this->_UpdatePos(cam);
    }

    void Sound::CameraMovement(Common::Camera const& cam)
    {
        this->_UpdatePos(cam);
    }

    void Sound::_UpdatePos(Common::Camera const& cam)
    {
        this->_channel->SetPosition(glm::fvec3(this->_pos - cam.position));
    }

}}}
