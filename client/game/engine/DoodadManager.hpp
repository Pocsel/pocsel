#ifndef __CLIENT_GAME_ENGINE_DOODADMANAGER_HPP__
#define __CLIENT_GAME_ENGINE_DOODADMANAGER_HPP__

namespace Client { namespace Game { namespace Engine {

    class Engine;
    class Doodad;

    class DoodadManager :
        private boost::noncopyable
    {
    private:
        Engine& _engine;
        Uint32 _runningDoodadId; // 0 quand aucun doodad n'est en cours d'éxécution
        Doodad* _runningDoodad; // nul quand aucun doodad n'est en cours d'éxécution

    public:
        DoodadManager(Engine& engine);
        Uint32 GetRunningDoodadId() const { return this->_runningDoodadId; }
        Uint32 GetRunningPluginId() const;
        Doodad const& GetDoodad(Uint32 doodadId) const throw(std::runtime_error); // ne pas garder la référence, le doodad peut etre delete à tout moment
    };

}}}

#endif
