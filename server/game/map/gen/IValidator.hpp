#ifndef __SERVER_GAME_MAP_GEN_IVALIDATOR_HPP__
#define __SERVER_GAME_MAP_GEN_IVALIDATOR_HPP__

#include "server/Logger.hpp"

namespace Server { namespace Game { namespace Map { namespace Gen {

    class IValidator
    {
    public:
        virtual bool Check(double x, double y, double z, double eq) const = 0;
        virtual ~IValidator()
        {
        }


        // XXX mode MethodPtr
//    public:
//        typedef bool (IValidator::*Checker)(double, double, double, double) const;
//
//    public:
//        Checker check;
//    private:
//        double _value;
//        double _a;
//        double _b;
//
//    public:
//        IValidator(Checker const& check, std::map<std::string, double> const& values) :
//            check(check)
//        {
//            if (values.find("value") != values.end())
//                _value = values.find("value")->second;
//            else
//                _value = 0;
//
//            if (values.find("a") != values.end())
//                _a = values.find("a")->second;
//            else
//                _a = 1;
//
//            if (values.find("b") != values.end())
//                _b = values.find("b")->second;
//            else
//                _b = 0;
//        }
//
//        bool EqInferiorTo(double, double, double, double eq) const
//        {
//            return eq < _value;
//        }
//        bool AYInferiorToEq(double, double y, double, double eq) const
//        {
//            return _a * y < eq;
//        }
//        bool YInferiorTo(double, double y, double, double) const
//        {
//            return y < _value;
//        }
//        bool AYPlusBInferiorToEq(double, double y, double, double eq) const
//        {
//            return _a * y + _b < eq;
//        }
//        bool EqInferiorToY(double, double y, double, double eq) const
//        {
//            return eq < y;
//        }
//        bool EqSuperiorTo(double, double, double, double eq) const
//        {
//            return eq > _value;
//        }
//        bool AYPlusBSuperiorToEq(double, double y, double, double eq) const
//        {
//            return _a * y + _b > eq;
//        }
//        bool AYSuperiorToEq(double, double y, double, double eq) const
//        {
//            return _a * y > eq;
//        }
//        bool YInferiorToEq(double, double y, double, double eq) const
//        {
//            return y < eq;
//        }
//        bool YSuperiorTo(double, double y, double, double) const
//        {
//            return y > _value;
//        }
    };

}}}}

#endif
