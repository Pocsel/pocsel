#ifndef __TOOLS_WINDOW_BINDACTION_HPP__
#define __TOOLS_WINDOW_BINDACTION_HPP__

// Je n'ai pas trouv� mieux pour g�rer l'enum "BindAction" suivant le projet
// TOOLS_WINDOW_BINDACTION_HEADER = "un/nom/de/fichier.hpp" (autre que "BindAction.hpp")
#ifdef TOOLS_WINDOW_BINDACTION_HEADER
# include TOOLS_WINDOW_BINDACTION_HEADER
#else // !TOOLS_WINDOW_BINDACTION_HPP
# error Il faut sp�cifier la macro "TOOLS_WINDOW_BINDACTION_HEADER"
#endif

namespace Tools { namespace Window { namespace BindAction {

    enum Type
    {
        Pressed,
        Held,
        Released,
    };

}}}

#endif
