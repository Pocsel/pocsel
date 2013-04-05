#ifndef __CLIENT_GAME_ENGINE_SOUNDMANAGER_HPP__
#define __CLIENT_GAME_ENGINE_SOUNDMANAGER_HPP__

#include <luasel/Luasel.hpp>

#include "tools/lua/AWeakResourceRef.hpp"
#include "tools/lua/WeakResourceRefManager.hpp"

namespace Client { namespace Game { namespace Engine {

    class Engine;
    class Sound;

    class SoundManager :
        private boost::noncopyable
    {
    public:
        struct WeakSoundRef : public Tools::Lua::AWeakResourceRef<SoundManager>
        {
            WeakSoundRef() : soundId(0) {}
            WeakSoundRef(Uint32 soundId) : soundId(soundId) {}
            virtual bool IsValid(SoundManager const&) const { return this->soundId != 0; }
            virtual void Invalidate(SoundManager const&) { this->soundId = 0; }
            virtual Luasel::Ref GetReference(SoundManager& soundManager) const;
            bool operator <(WeakSoundRef const& rhs) const;
            Uint32 soundId;
        };

    private:
        Engine& _engine;
        std::map<Uint32 /* soundId */, Sound*> _sounds;
        std::map<Uint32 /* doodadId */, std::list<Sound*>> _soundsByDoodad;
        Uint32 _nextSoundId;
        Tools::Lua::WeakResourceRefManager<WeakSoundRef, SoundManager>* _weakSoundRefManager;

    public:
        SoundManager(Engine& engine);
        ~SoundManager();
        void Tick(Uint64 totalTime);
        void DeleteSoundsOfDoodad(Uint32 doodadId);
        Luasel::Ref GetLuaWrapperForSound(Uint32 soundId);
    private:
        Sound const& _GetSound(Uint32 soundId) const throw(std::runtime_error);
        Uint32 _RefToSoundId(Luasel::Ref const& ref) const throw(std::runtime_error);
        void _ApiSpawn(Luasel::CallHelper& helper);
        void _ApiKill(Luasel::CallHelper& helper);
        void _ApiPlay(Luasel::CallHelper& helper);
    };

}}}

#endif
