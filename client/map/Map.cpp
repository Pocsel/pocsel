#include "client/precompiled.hpp"

#include "client/game/Game.hpp"
#include "client/map/Map.hpp"

#include "common/CubePosition.hpp"

namespace Client { namespace Map {

    Map::Map(Game::Game& game, std::string const& name)
        : _game(game),
        _chunkManager(game),
        _name(name)
    {
    }

    float Map::GetLoadingProgression() const
    {
        return this->_chunkManager.GetLoadingProgression();
    }

    bool Map::GetFirstCube(std::vector<Common::CubePosition> const& positions, Common::CubePosition& result)
    {
        std::shared_ptr<Chunk> curChunk;

        for (auto it = positions.begin(), ite = positions.end(); it != ite; ++it)
        {
            Common::CubePosition const& pos = *it;
            Chunk::IdType id = Chunk::CoordsToId(pos.world);
            if (curChunk.get() == 0 || curChunk->id != id)
                curChunk = this->_chunkManager.GetChunk(id);

            if (curChunk == 0)
                continue;

            if (curChunk->GetCube(pos.chunk.x, pos.chunk.y, pos.chunk.z) != 0)
            {
                result = pos;
                return true;
            }
        }

        return false;
    }

}}
