#ifndef __CLIENT_BINDACTION_HPP__
#define __CLIENT_BINDACTION_HPP__

namespace Client { namespace BindAction {

    enum Type
    {
        Pressed,
        Held,
        Released,
    };

    enum BindAction
    {
        None,
        Quit,
        Menu,
        Forward,
        Backward,
        Left,
        Right,
        Jump,
        Fire,
        SecondaryFire,
        Use,
        // Pour les tableaux
        NbBindActions
    };

}}

#endif
