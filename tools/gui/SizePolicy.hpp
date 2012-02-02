#ifndef __TOOLS_GUI_SIZEPOLICY_HPP__
#define __TOOLS_GUI_SIZEPOLICY_HPP__

#include "tools/enum.hpp"

namespace Tools { namespace Gui {

    struct SizePolicy
    {
    public:
        ENUM Constraint { Fixed, AtLeast, AtMost, Floating };
        ENUM Unit { Pixel, Percent };

    public:
        unsigned int width, height;
        Constraint widthConstraint, heightConstraint;
        Unit widthUnit, heightUnit;

    public:
        SizePolicy(Constraint constraint, Unit unit, unsigned int width, unsigned int height) :
            width(width), height(height),
            widthConstraint(constraint), heightConstraint(constraint),
            widthUnit(unit), heightUnit(unit)

        {}
        SizePolicy() :
            width(100), height(100),
            widthConstraint(Constraint::Floating), heightConstraint(Constraint::Floating),
            widthUnit(Unit::Percent), heightUnit(Unit::Percent)
        {}
    };

}}

#endif
