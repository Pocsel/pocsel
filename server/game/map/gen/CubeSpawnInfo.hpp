#ifndef __SERVER_GAME_MAP_GEN_CUBESPAWNINFO_HPP__
#define __SERVER_GAME_MAP_GEN_CUBESPAWNINFO_HPP__

#include "server2/Chunk.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    class IValidator;

    class CubeSpawnInfo
    {
        public:
            struct Validation
            {
                unsigned int equationIndex;
                IValidator* validator;
            };
        public:
            Chunk::CubeType const cubeType;

        protected:
            std::list<Validation> _validations;

        public:
            explicit CubeSpawnInfo(Chunk::CubeType cubeType, std::list<Validation> const& validations);
            ~CubeSpawnInfo();

            std::list<Validation> const& GetValidations() const { return _validations; }
    };

}}}}

#endif
