#!/bin/sh

MODULENAME=$1

show_help()
{
    echo "\033[0;31;1minvalid parameter !\033[0m"
    echo "eg: sh stop.sh modulename"
    echo "module: 1. dispatcher"
}

stopDispatcher()
{
    PID=`ps -ef | grep dispatcher | grep -v 'grep\|stop.sh\|restart.sh' | awk '{print $2}'`
    if [ "$PID" != "" ];then
        kill -9 $PID
    fi

    echo "Stop dispatcher Success"
}


case $MODULENAME in
    dispatcher)
        stopDispatcher
        ;;
    *)
        show_help
        ;;
esac