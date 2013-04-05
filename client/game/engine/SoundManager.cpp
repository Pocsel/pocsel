#include "client/game/engine/SoundManager.hpp"
#include "client/game/engine/Sound.hpp"
#include "client/game/engine/Engine.hpp"
#include "client/game/Game.hpp"

namespace Client { namespace Game { namespace Engine {

    SoundManager::SoundManager(Engine& engine) :
        _engine(engine),
        _nextSoundId(1) // le premier son sera le 1, 0 est la valeur spéciale "pas de son"
    {
        auto& i = this->_engine.GetInterpreter();

        this->_weakSoundRefManager = new Tools::Lua::WeakResourceRefManager<WeakSoundRef, SoundManager>(
                i, /* interpreter */
                *this, /* resource manager */
                false /* use fake references TODO recevoir ce bool depuis le reseau */);

        auto namespaceTable = i.Globals().GetTable("Client").GetTable("Sound");
        namespaceTable.Set("Spawn", i.MakeFunction(std::bind(&SoundManager::_ApiSpawn, this, std::placeholders::_1)));
        namespaceTable.Set("Kill", i.MakeFunction(std::bind(&SoundManager::_ApiKill, this, std::placeholders::_1)));
        namespaceTable.Set("Play", i.MakeFunction(std::bind(&SoundManager::_ApiPlay, this, std::placeholders::_1)));
    }

    SoundManager::~SoundManager()
    {
        auto itSound = this->_sounds.begin();
        auto itSoundEnd = this->_sounds.end();
        for (; itSound != itSoundEnd; ++itSound)
            Tools::Delete(itSound->second);
    }

    Luasel::Ref SoundManager::WeakSoundRef::GetReference(SoundManager& soundManager) const
    {
        return soundManager.GetLuaWrapperForSound(this->soundId);
    }

    bool SoundManager::WeakSoundRef::operator <(WeakSoundRef const& rhs) const
    {
        return this->soundId < rhs.soundId;
    }

    void SoundManager::Tick(Uint64 totalTime)
    {
        // TODO ne pas faire cette merde inéficiente
        auto const& doodads = this->_engine.GetDoodadManager().GetDoodads();
        for (auto const& it : doodads)
        {
        }
    }

    void SoundManager::DeleteSoundsOfDoodad(Uint32 doodadId)
    {
        auto listIt = this->_soundsByDoodad.find(doodadId);
        if (listIt == this->_soundsByDoodad.end())
            return; // le doodad n'a aucun son
        for (auto& it : listIt->second)
        {
            this->_sounds.erase((*it)->GetId());
            Tools::Delete(*it);
        }
        this->_soundsByDoodad.erase(listIt);
    }

    Luasel::Ref SoundManager::GetLuaWrapperForSound(Uint32 soundId)
    {
        auto& i = this->_engine.GetInterpreter();
        auto object = i.MakeTable();
        object.Set("id", soundId);
        object.Set("Kill", i.MakeFunction(std::bind(&SoundManager::_ApiKill, this, std::placeholders::_1)));
        object.Set("Play", i.MakeFunction(std::bind(&SoundManager::_ApiPlay, this, std::placeholders::_1)));
        return object;
    }

    Sound const& SoundManager::_GetSound(Uint32 soundId) const throw(std::runtime_error)
    {
        auto it = this->_sounds.find(soundId);
        if (it == this->_sounds.end())
            throw std::runtime_error("SoundManager::_GetSound: Sound " + Tools::ToString(soundId) + " not found.");
        return *it->second;
    }

    Uint32 SoundManager::_RefToSoundId(Luasel::Ref const& ref) const throw(std::runtime_error)
    {
        if (ref.IsNumber()) /* id directement en nombre */
            return ref.To<Uint32>();
        else if (ref.IsTable()) /* type final sound wrapper (weak pointer déréférencé) */
            return ref["id"].Check<Uint32>("SoundManager::_RefToSoundId: Table argument has no id number field");
        else if (ref.IsUserData())
        {
            if (this->_weakSoundRefManager->UsingFakeReferences() && ref.Is<Tools::Lua::WeakResourceRefManager<WeakSoundRef, SoundManager>::FakeReference*>()) /* sound wrapper en fake reference (weak pointer déréférencé) */
            {
                auto fakeRef = ref.To<Tools::Lua::WeakResourceRefManager<WeakSoundRef, SoundManager>::FakeReference*>();
                if (fakeRef->IsValid())
                {
                    auto trueRef = fakeRef->GetReference();
                    if (trueRef.IsTable())
                        return trueRef["id"].Check<Uint32>("SoundManager::_RefToSoundId: Table argument has no id number field");
                    else
                        throw std::runtime_error("SoundManager::_RefToSoundId: Fake reference does not contain a table but a " + trueRef.GetTypeName() + ", is this really a sound?");
                }
                else
                    throw std::runtime_error("SoundManager::_RefToSoundId: This reference was invalidated - you must not keep true references to sounds, only weak references");
            }
            else /* weak pointer (non déréférencé) */
            {
                WeakSoundRef* m = ref.Check<WeakSoundRef*>("SoundManager::_RefToSoundId: Userdata argument is not of WeakSoundRef type");
                if (m->IsValid(*this))
                    return m->soundId;
                else
                    throw std::runtime_error("SoundManager::_RefToSoundId: This weak pointer was invalidated - the sound does not exist anymore");
            }
        }
        else
            throw std::runtime_error("SoundManager::_RefToSoundId: Invalid argument type " + ref.GetTypeName() + " given");
    }

    void SoundManager::_ApiSpawn(Luasel::CallHelper& helper)
    {
        Uint32 doodadId = this->_engine.GetRunningDoodadId();
        if (helper.GetNbArgs() >= 2)
            doodadId = helper.PopArg().Check<Uint32>("Client.Sound.Spawn: Argument \"doodadId\" must be a number");
        std::string soundName = helper.PopArg("Client.Sound.Spawn: Missing argument \"soundName\"").CheckString("Client.Sound.Spawn: Argument \"soundName\" must be a string");
        if (!doodadId)
        {
            Tools::error << "SoundManager::_ApiSpawn: No doodad with id " << doodadId << ", cannot spawn sound." << std::endl;
            return; // retourne nil
        }

        std::shared_ptr<Tools::Sound::ISound> soundResource;
        try
        {
            soundResource = this->_engine.GetGame().GetResourceManager().GetSound(soundName);
        }
        catch (std::exception& e)
        {
            Tools::error << "SoundManager::_ApiSpawn: Could not get sound \"" << soundName << "\": " << e.what() << std::endl;
            return; // retourne nil
        }

        // trouve le prochain soundId
        while (!this->_nextSoundId // 0 est la valeur spéciale "pas de son", on la saute
                || this->_sounds.count(this->_nextSoundId))
            ++this->_nextSoundId;
        Uint32 newId = this->_nextSoundId++;

        Sound* s = new Sound(newId, doodadId, soundResource);
        this->_sounds[newId] = s;
        this->_soundsByDoodad[doodadId].push_back(s);
        helper.PushRet(this->_engine.GetInterpreter().MakeNumber(newId));
    }

    void SoundManager::_ApiKill(Luasel::CallHelper& helper)
    {
        // trouve le son
        Uint32 soundId = this->_RefToSoundId(helper.PopArg("Client.Sound.Kill: Missing argument \"sound\""));
        auto it = this->_sounds.find(soundId);
        if (it == this->_sounds.end())
        {
            Tools::error << "SoundManager::_ApiKill: No sound with id " << soundId << ", cannot kill sound." << std::endl;
            return;
        }

        // enleve le son de la liste associée au doodad
        auto itList = this->_soundsByDoodad.find(it->second->GetDoodadId());
        assert(itList != this->_soundsByDoodad.end() && "un son de la map générale des son n'était pas dans une liste associée a un doodad");
        itList->second.remove(it->second);
        if (itList->second.empty())
            this->_soundsByDoodad.erase(itList); // supprime l'entrée associée au doodad si celui-ci n'a plus de son

        // enleve le son de la map générale + delete
        Tools::Delete(it->second);
        this->_sounds.erase(it);
    }

    void SoundManager::_ApiPlay(Luasel::CallHelper& helper)
    {
        Sound const& s = this->_GetSound(this->_RefToSoundId(helper.PopArg("Client.Sound.Play: Missing argument \"sound\"")));
        s.Play();
    }

}}}
