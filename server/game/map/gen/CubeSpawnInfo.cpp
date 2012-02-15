#include "server/game/map/gen/CubeSpawnInfo.hpp"
#include "server/game/map/gen/IValidator.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    CubeSpawnInfo::CubeSpawnInfo(Chunk::CubeType cubeType, std::vector<Validation> const& validations) :
        cubeType(cubeType),
        _validations(validations)
    {
        this->_Init();
    }

    CubeSpawnInfo::CubeSpawnInfo(CubeSpawnInfo const& other) :
        cubeType(other.cubeType),
        _validations(other._validations)
    {
        const_cast<CubeSpawnInfo&>(other)._validations.clear();
        this->_Init();
    }

    CubeSpawnInfo const& CubeSpawnInfo::operator=(CubeSpawnInfo const& other)
    {
        const_cast<Chunk::CubeType&>(cubeType) = other.cubeType;
        this->_validations = other._validations;
        const_cast<CubeSpawnInfo&>(other)._validations.clear();
        this->_Init();
        return *this;
    }

    CubeSpawnInfo::~CubeSpawnInfo()
    {
        for (auto it = _validations.begin(), ite = _validations.end(); it != ite; ++it)
            Tools::Delete(it->validator);
    }

}}}}
