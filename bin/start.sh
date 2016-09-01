#!/bin/sh

export LD_PRELOAD=../lib/libjemalloc.so
export LD_LIBRARY_PATH=../lib/:$LD_LIBRARY_PATH

PID=`ps -ef | grep DCFabric | grep -v grep | awk '{print $2}'`
if [ "$PID" = "" ];then
	./DCFabric &
	echo "Startup DCFabric Success"
    exit 0
else
	echo "DCFabric is already exist, Startup Failure!"
    exit 1
fi
