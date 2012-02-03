#include <iostream>

#include "server/mapgen/CubeSpawnInfo.hpp"
#include "server/mapgen/Validator.hpp"

namespace Server { namespace MapGen {

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

}}
