#ifndef __CLIENT_GAME_SOUND_ENGINE_HPP__
#define __CLIENT_GAME_SOUND_ENGINE_HPP__

#include "common/Position.hpp"

namespace Tools { namespace Sound {
    class ISound;
    class AChannel;
}}
namespace Common {
    struct Camera;
}

namespace Client { namespace Game { namespace Engine {

    class Engine;
    class SoundManager;

    class Sound :
        private boost::noncopyable
    {
    private:
        Engine& _engine;
        Uint32 _id;
        Uint32 _doodadId;
        std::shared_ptr<Tools::Sound::ISound> _sound;
        std::unique_ptr<Tools::Sound::AChannel> _channel;
        Common::Position _pos;
        Uint32 _weakReferenceId;

    public:
        Sound(Engine& engine, Uint32 id, Uint32 doodadId, std::shared_ptr<Tools::Sound::ISound> sound, Common::Position const& pos);
        ~Sound();
        Uint32 GetId() const { return this->_id; }
        Uint32 GetDoodadId() const { return this->_doodadId; }
        void Play() const;
        void SetPosition(glm::fvec3 const& pos, Common::Camera const& cam);
        void CameraMovement(Common::Camera const& cam); // notification du movement de la camera du joueur
        Uint32 GetWeakReferenceId() const { return this->_weakReferenceId; }
    private:
        void _UpdatePos(Common::Camera const& cam);
    };

}}}

#endif
