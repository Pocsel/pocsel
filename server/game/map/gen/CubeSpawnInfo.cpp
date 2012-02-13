#include "server/game/map/gen/CubeSpawnInfo.hpp"
#include "server/game/map/gen/IValidator.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    CubeSpawnInfo::CubeSpawnInfo(Chunk::CubeType cubeType, std::list<Validation> const& validations) :
        cubeType(cubeType),
        _validations(validations)
    {
    }

    CubeSpawnInfo::~CubeSpawnInfo()
    {
        for (auto it = _validations.begin(), ite = _validations.end(); it != ite; ++it)
            Tools::Delete(it->validator);
    }

}}}}
