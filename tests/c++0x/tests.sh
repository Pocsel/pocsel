#!/bin/sh

SCRIPT="$0"
SCRIPTDIR=`python -c "import os;print os.path.abspath(os.path.dirname('$SCRIPT'))"`
DEFAULT_COMPILER=g++

[ -z "$COMPILER" ] && COMPILER=$DEFAULT_COMPILER

$COMPILER -v
echo "C++0x implemented features for '$COMPILER':"

for SNIPPET in "$SCRIPTDIR"/*.cpp
do
    echo -n "  - $SNIPPET: "
    ( $COMPILER -c -std=c++0x "$SNIPPET" 2>&1 ) > /dev/null
    [ $? = 0 ] && echo Success || echo Fail
done

rm -f "$SCRIPTDIR"/*.o
