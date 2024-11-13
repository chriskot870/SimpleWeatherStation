#!/bin/bash
#
# This starts the gdbserver on the target machine
#
# Load the gdbserver data
#
. cmake/gdbserver.config
#
# Check if the gdb server is running. The gdbserver should be stopped
# when the debugger is stopped. But if it isn't for some reason we
# kill it now.
#
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