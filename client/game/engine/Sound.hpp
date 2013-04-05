#ifndef __CLIENT_GAME_SOUND_ENGINE_HPP__
#define __CLIENT_GAME_SOUND_ENGINE_HPP__

namespace Tools { namespace Sound {
    class ISound;
}}

namespace Client { namespace Game { namespace Engine {

    class Sound :
        private boost::noncopyable
    {
    private:
        Uint32 _id;
        Uint32 _doodadId;
        std::shared_ptr<Tools::Sound::ISound> _sound;
        std::unique_ptr<Tools::Sound::AChannel> _channel;

    public:
        Sound(Uint32 id, Uint32 doodadId, std::shared_ptr<Tools::Sound::ISound> sound);
        ~Sound();
        Uint32 GetId() const { return this->_id; }
        Uint32 GetDoodadId() const { return this->_doodadId; }
        void Play() const;
        void SetPosition(glm::fvec3 const& pos);
    };

}}}

#endif
