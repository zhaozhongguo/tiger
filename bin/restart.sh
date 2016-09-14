#!/bin/sh

MODULENAME=$1

show_help()
{
    echo "\033[0;31;1minvalid parameter !\033[0m"
    echo "eg: sh restart.sh modulename"
    echo "module: 1. dispatcher"
}

restartDispatcher()
{
    sh stop.sh dispatcher
    sh start.sh dispatcher
}


case $MODULENAME in
    dispatcher)
        restartDispatcher
        ;;
    *)
        show_help
        ;;
esac