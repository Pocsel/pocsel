#ifndef __SERVER_GAME_MAP_GEN_CUBESPAWNINFO_HPP__
#define __SERVER_GAME_MAP_GEN_CUBESPAWNINFO_HPP__

#include "server/Chunk.hpp"

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
            std::vector<Validation> _validations;
            Validation* _valBegin;
            Validation* _valEnd;

        public:
            explicit CubeSpawnInfo(Chunk::CubeType cubeType, std::vector<Validation> const& validations);
            explicit CubeSpawnInfo(CubeSpawnInfo const& other);
            CubeSpawnInfo const& operator=(CubeSpawnInfo const& other);
            ~CubeSpawnInfo();

            std::vector<Validation> const& GetValidations() const { return _validations; }
            void _Init()
            {
                this->_valBegin = this->_validations.data();
                this->_valEnd = this->_validations.data() + this->_validations.size();
            }
            Validation const* GetValBegin() const { return this->_valBegin; }
            Validation const* GetValEnd() const { return this->_valEnd; }
    };

}}}}

#endif
