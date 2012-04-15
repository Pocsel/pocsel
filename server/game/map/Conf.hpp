#ifndef __SERVER_GAME_MAP_CONF_HPP__
#define __SERVER_GAME_MAP_CONF_HPP__

namespace Server { namespace Game { namespace Map {

    struct CubeType;

    struct Conf
    {
        std::string name;
        std::string fullname;
        bool is_default;
        struct EquationConf
        {
            std::string function_name;
            std::map<std::string, double> values;
        };
        std::map<std::string, EquationConf> equations;
        struct ValidatorConf
        {
            std::string equation;
            std::string validator;
            std::map<std::string, double> values;
        };
        struct ValidationBlocConf
        {
            int priority;
            CubeType const* cube_type; // ça se répète avec celui de CubeConf mais c'est fait exprès
            std::list<ValidatorConf> validators;
        };
        struct CubeConf
        {
            CubeType const* type;
            std::list<ValidationBlocConf> validation_blocs;
        };
        std::map<std::string, CubeConf> cubes;
        std::vector<CubeType> cubeTypes;
    };

}}}

#endif
