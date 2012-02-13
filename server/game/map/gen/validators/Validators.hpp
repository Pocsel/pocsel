#ifndef __SERVER_GAME_MAP_GEN_VALIDATORS_VALIDATORS_HPP__
#define __SERVER_GAME_MAP_GEN_VALIDATORS_VALIDATORS_HPP__

#include "server/game/map/gen/validators/EqSuperiorTo.hpp"
#include "server/game/map/gen/validators/EqInferiorTo.hpp"

#include "server/game/map/gen/validators/YSuperiorTo.hpp"
#include "server/game/map/gen/validators/YInferiorTo.hpp"

#include "server/game/map/gen/validators/YInferiorToEq.hpp"
#include "server/game/map/gen/validators/EqInferiorToY.hpp"

#include "server/game/map/gen/validators/AYInferiorToEq.hpp"
#include "server/game/map/gen/validators/AYPlusBInferiorToEq.hpp"

#include "server/game/map/gen/validators/AYSuperiorToEq.hpp"
#include "server/game/map/gen/validators/AYPlusBSuperiorToEq.hpp"

#include "tools/CaseInsensitive.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    class Perlin;

}}}}

namespace Server { namespace Game { namespace Map { namespace Gen { namespace Validators {

    class Validators
    {
    private:
        std::map<std::string, IValidator* (Validators::*)(std::map<std::string, double> const&), Tools::CaseInsensitive> _validatorCreators;

    public:

        IValidator* CreateValidator(std::string const& name, std::map<std::string, double> const& values)
        {
            if (this->_validatorCreators.find(name) == this->_validatorCreators.end())
                return 0;
            return (this->*(this->_validatorCreators[name]))(values);
        }

#define CREATE_THIS_ONE(val) IValidator* _Create##val(std::map<std::string, double> const& values) \
        { \
            return new val(values); \
        } \

        CREATE_THIS_ONE(EqSuperiorTo)
        CREATE_THIS_ONE(EqInferiorTo)
        CREATE_THIS_ONE(YSuperiorTo)
        CREATE_THIS_ONE(YInferiorTo)
        CREATE_THIS_ONE(YInferiorToEq)
        CREATE_THIS_ONE(EqInferiorToY)
        CREATE_THIS_ONE(AYInferiorToEq)
        CREATE_THIS_ONE(AYPlusBInferiorToEq)
        CREATE_THIS_ONE(AYSuperiorToEq)
        CREATE_THIS_ONE(AYPlusBSuperiorToEq)

#undef CREATE_THIS_ONE

#define ADD_THIS_ONE(str, val) this->_validatorCreators[str] = &Validators::_Create##val

        Validators()
        {
            ADD_THIS_ONE("eq_superior", EqSuperiorTo);
            ADD_THIS_ONE("eq_superior_to", EqSuperiorTo);
            ADD_THIS_ONE("eqsuperior", EqSuperiorTo);
            ADD_THIS_ONE("eqsuperiorto", EqSuperiorTo);

            ADD_THIS_ONE("eq_inferior", EqInferiorTo);
            ADD_THIS_ONE("eq_inferior_to", EqInferiorTo);
            ADD_THIS_ONE("eqinferior", EqInferiorTo);
            ADD_THIS_ONE("eqinferiorto", EqInferiorTo);

            ADD_THIS_ONE("y_superior", YSuperiorTo);
            ADD_THIS_ONE("y_superior_to", YSuperiorTo);
            ADD_THIS_ONE("ysuperior", YSuperiorTo);
            ADD_THIS_ONE("ysuperiorto", YSuperiorTo);

            ADD_THIS_ONE("y_inferior", YInferiorTo);
            ADD_THIS_ONE("y_inferior_to", YInferiorTo);
            ADD_THIS_ONE("yinferior", YInferiorTo);
            ADD_THIS_ONE("yinferiorto", YInferiorTo);

            ADD_THIS_ONE("y_inferior_to_eq", YInferiorToEq);
            ADD_THIS_ONE("y_inferior_eq", YInferiorToEq);
            ADD_THIS_ONE("yinferiortoeq", YInferiorToEq);
            ADD_THIS_ONE("yinferioreq", YInferiorToEq);
            ADD_THIS_ONE("y<eq", YInferiorToEq);
            ADD_THIS_ONE("eq_superior_to_y", YInferiorToEq);
            ADD_THIS_ONE("eq_superior_y", YInferiorToEq);
            ADD_THIS_ONE("eqsuperiortoy", YInferiorToEq);
            ADD_THIS_ONE("eqsuperiory", YInferiorToEq);
            ADD_THIS_ONE("eq>y", YInferiorToEq);

            ADD_THIS_ONE("eq_inferior_to_y", EqInferiorToY);
            ADD_THIS_ONE("eq_inferior_y", EqInferiorToY);
            ADD_THIS_ONE("eqinferiortoy", EqInferiorToY);
            ADD_THIS_ONE("eqinferiory", EqInferiorToY);
            ADD_THIS_ONE("eq<y", EqInferiorToY);
            ADD_THIS_ONE("y_superior_to_eq", EqInferiorToY);
            ADD_THIS_ONE("y_superior_eq", EqInferiorToY);
            ADD_THIS_ONE("ysuperiortoeq", EqInferiorToY);
            ADD_THIS_ONE("ysuperioreq", EqInferiorToY);
            ADD_THIS_ONE("y>eq", EqInferiorToY);

            ADD_THIS_ONE("ay+b<eq", AYPlusBInferiorToEq);
            ADD_THIS_ONE("ay<eq", AYInferiorToEq);

            ADD_THIS_ONE("ay+b>eq", AYPlusBSuperiorToEq);
            ADD_THIS_ONE("ay>eq", AYSuperiorToEq);
        }
#undef ADD_THIS_ONE
    };

}}}}}

#endif
