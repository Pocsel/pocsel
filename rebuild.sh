#!/bin/sh

# sudo apt-get install ccache :)
# c'est un petit programme qui permet d'accélérer de BEAUCOUP la compil !
# de plus, on peut faire -j4 avec, et ça MARCHE, pas comme avec le header précompilé !
# .. c'est vraiment de la merde make, et gcc aussi.
# et toi, tu fais quoi avec TAMERD ?

SCRIPT="$0"
SCRIPTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$SCRIPT')))"`

if [ -d "$SCRIPTDIR"/gcc/install ]
then
        PATH="$SCRIPTDIR/gcc/install/bin":$PATH
        LD_LIBRARY_PATH="$SCRIPTDIR/gcc/install/lib:$LD_LIBRARY_PATH"
fi

mkdir -p "$SCRIPTDIR"/build
cd "$SCRIPTDIR"/build
PATH=/usr/lib/ccache:"$PATH" cmake .. || exit 1

if [ -z "$NB_THREADS" ]
then
    NB_CORES=`cat /proc/cpuinfo | grep processor | wc -l`
    NB_THREADS=`expr $NB_CORES + 1`
fi

make -j$NB_THREADS

