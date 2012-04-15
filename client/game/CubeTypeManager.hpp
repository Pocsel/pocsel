#ifndef __CLIENT_GAME_CUBETYPEMANAGER_HPP__
#define __CLIENT_GAME_CUBETYPEMANAGER_HPP__

#include "common/BaseChunk.hpp"
#include "client/game/CubeType.hpp"

namespace Client {
    class Client;
}

namespace Client { namespace Game {

    class CubeTypeManager
    {
    private:
        Common::BaseChunk::CubeType _nbCubeTypes;
        Common::BaseChunk::CubeType _curAskedType;
        std::vector<CubeType> _cubeTypes;
        Client& _client;

    public:
        CubeTypeManager(Client& client, Common::BaseChunk::CubeType nbCubeTypes);

        void AddCubeType(std::unique_ptr<Common::CubeType> cubeType);
        void LoadResources();

        std::vector<CubeType> const& GetCubeTypes() const { return this->_cubeTypes; }
        float GetLoadingProgression() const { return this->_cubeTypes.size() / (float)this->_nbCubeTypes; }
    private:
        void _AskOneType();
    };

}}

#endif
