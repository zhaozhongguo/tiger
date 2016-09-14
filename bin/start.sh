#!/bin/sh

MODULENAME=$1

export LD_LIBRARY_PATH=../lib/:$LD_LIBRARY_PATH

show_help()
{
    echo "\033[0;31;1minvalid parameter !\033[0m"
    echo "eg: sh start.sh modulename"
    echo "module: 1. dispatcher"
}

startDispatcher()
{
    PID=`ps -ef | grep dispatcher | grep -v 'grep\|start.sh\|restart.sh' | awk '{print $2}'`
    if [ "$PID" = "" ];then
        ./dispatcher
        echo "Startup dispatcher Success"
    else
        echo "dispatcher is already up, Startup Failure!"
    fi
}


case $MODULENAME in
    dispatcher)
        startDispatcher
        ;;
    *)
        show_help
        ;;
esac

