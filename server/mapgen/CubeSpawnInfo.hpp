#ifndef __SERVER_MAPGEN_CUBESPAWNINFO_HPP__
#define __SERVER_MAPGEN_CUBESPAWNINFO_HPP__

#include <list>
#include "server/Chunk.hpp"

namespace Server { namespace MapGen {

    class Validator;

    class CubeSpawnInfo
    {
        public:
            struct Validation
            {
                unsigned int equationIndex;
                Validator* validator;
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

}}

#endif
