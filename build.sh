#!/bin/sh

SCRIPT="$0"
SCRIPTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$SCRIPT')))"`

mkdir -p "$SCRIPTDIR"/build
cd "$SCRIPTDIR"/build

if [ $# -ge 1 ]; then
    if [ $# -ge 2 ]; then
        cmake -D CMAKE_BUILD_TYPE:STRING=$1 -D CMAKE_CXX_COMPILER:FILEPATH=$2 .. || exit 1
    else
        cmake -D CMAKE_BUILD_TYPE:STRING=$1 .. || exit 1
    fi
else
    cmake .. || exit 1
fi

if [ -z "$NB_THREADS" ]
then
    NB_CORES=`cat /proc/cpuinfo | grep processor | wc -l`
    NB_THREADS=`expr $NB_CORES + 1`
fi

make -j$NB_THREADS
