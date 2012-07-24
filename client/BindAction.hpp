#ifndef __CLIENT_BINDACTION_HPP__
#define __CLIENT_BINDACTION_HPP__

namespace Tools { namespace Window { namespace BindAction {

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
        Crouch,
        Fire,
        AltFire,
        Use,
        ToggleSprint,
        ToggleDrawPhysics,
        // Pour les tableaux
        NbBindActions
    };

}}}

#endif
