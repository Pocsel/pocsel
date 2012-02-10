#ifndef __CLIENT_BINDACTION_HPP__
#define __CLIENT_BINDACTION_HPP__

namespace Client {

    namespace BindAction {
        enum BindAction
        {
            None,
            Forward,
            Backward,
            Left,
            Right,
            Jump,
            Fire,
            SecondaryFire,
            Use,
        };
    }

}

#endif
