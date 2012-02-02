#ifndef __SERVER_MAPGEN_CUBESPAWNINFO_HPP__
#define __SERVER_MAPGEN_CUBESPAWNINFO_HPP__

#include <list>
#include "server/Chunk.hpp"

namespace Server
{
    namespace MapGen
    {

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
//            unsigned int const equationIndex; // index dans _equations de MapGenerator XXX OLD
            Chunk::CubeType const cubeType;

        protected:
//            Validator* _validator; // XXX OLD
            std::list<Validation> _validations;

        public:
            // XXX OLD
//            explicit CubeSpawnInfo(Chunk::CubeType cubeType, unsigned int equationIndex, Validator* validator);
            explicit CubeSpawnInfo(Chunk::CubeType cubeType, std::list<Validation> const& validations);
            ~CubeSpawnInfo();

            std::list<Validation> const& GetValidations() const { return _validations; }

//            Validator const& GetValidator() const { return *_validator; } // XXX OLD

        };

    }
}

#endif
