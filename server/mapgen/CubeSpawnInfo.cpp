#include <iostream>
#include "server/mapgen/CubeSpawnInfo.hpp"
#include "server/mapgen/Validator.hpp"

namespace Server
{
    namespace MapGen
    {

        // XXX OLD
//        CubeSpawnInfo::CubeSpawnInfo(Chunk::CubeType cubeType, unsigned int equationIndex, Validator* validator) :
//            equationIndex(equationIndex),
//            cubeType(cubeType),
//            _validator(validator)
//        {
//        }

        CubeSpawnInfo::CubeSpawnInfo(Chunk::CubeType cubeType, std::list<Validation> const& validations) :
            cubeType(cubeType),
            _validations(validations)
        {
        }

        CubeSpawnInfo::~CubeSpawnInfo()
        {
            for (auto it = _validations.begin(), ite = _validations.end(); it != ite; ++it)
                Tools::Delete(it->validator);
//            Tools::Delete(_validator); // XXX OLD
        }

        // XXX OLD
//        bool CubeSpawnInfo::Check(double x, double y, double z, double h, double empty) const
//        {
//            switch (this->_type)
//            {
//                case 5: // water
//                    if (y < 28 && (empty > 0.19 || y > h))
//                        return true;
//                    break;
//                case 0: // void (air)
//                    if (empty > 0.19 || y > h)
//                            return true;
//                    break;
//                case 1: // herbe
//                    if (h - 1.0 / (double)Common::ChunkSize < y && y <= h)
//                        return true;
//                    break;
//                case 2: // terre
//                    if (y < h)
//                        return true;
//                    break;
//                case 3: // cailloux
//                    if (y < h - 1)
//                        return true;
//                    break;
//                case 4: // fer
//                    if (empty > 0.15 && empty < 0.17)
//                        return true;
//                    break;
//            }
//            return false;
//        }

    }
}
