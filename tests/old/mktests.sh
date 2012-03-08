#!/bin/sh

SCRIPT="$0"
SCRIPTDIR=`python -c "import os;print os.path.abspath(os.path.dirname('$SCRIPT'))"`

test -d "$SCRIPTDIR/build" || mkdir "$SCRIPTDIR/build"

( cd build && cmake "$SCRIPTDIR" && make ) || exit 1

TESTS=""

for f in "$SCRIPTDIR"/build/*
do
    [ -x "$f" -a -f "$f" ] && TESTS="$TESTS $f"
done

printError()
{
    echo "\033[31;2mERROR:\033[0m $*"
}

printSuccess()
{
    echo "\033[32;2mSUCCESS:\033[0m $*"
}


printWarning()
{
    echo "\033[33mWARNING:\033[0m $*"
}

printInfo()
{
    echo -n "\033[34;1m$*\033[0m"
}

printResults()
{
    local msg success total
    success=`( test -z $1 && echo 0 ) || echo $1`
    total=`( test -z $2 && echo 0 ) || echo $2`
    msg="passed $success / $total"
    [ $total = 0 ] && echo "No test done" && return 1
    if [ $success = $total ]
    then
        printSuccess "$msg"
    else
        if [ $success = 0 ]
        then
            printError "$msg:"
        else
            printWarning "$msg:"
        fi
    fi
}

execTest()
{
    local msg res success total
    msg=`exec "$1" 2>&1`
    res=$?
    if [ $res = 0 ]
    then
        lastLine="`echo \"$msg\" | tail -n1`"
        success=`echo $lastLine | cut -f1 -d' '`
        total=`echo $lastLine | cut -f2 -d' '`
        printResults $success $total
        echo "$msg" | head -n '-1'
    else
        printError "$msg"
    fi
}

cd "$SCRIPTDIR/build/"

for TEST in $TESTS
do
    printInfo "* Starting tests for $TEST: "
    EXEC="$TEST"
    if [ ! -x "$EXEC" ]
    then
        printError "Cannot find test program $EXEC"
    else
        execTest "$EXEC"
    fi
done
