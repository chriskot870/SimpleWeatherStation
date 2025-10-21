#!/bin/bash

target_ip="192.168.50.6"
target_user="chrisk"

gdb_server_pid=`ssh $target_user@$target_ip pgrep -x gdbserver`

#
# If it is running kill it
#
if [ ! -z $gdb_server_pid ]
then
    echo Killing the gdbserver on the target pid $gdb_server_pid
    ssh $target_user@$target_ip kill $gdb_server_pid
else
    echo gdbserver was not running on the target
fi

exit 0