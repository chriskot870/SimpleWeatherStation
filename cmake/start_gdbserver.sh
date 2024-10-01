#!/bin/bash
#
# This starts the gdbserver on the target machine
#
# Load the gdbserver data
#
. cmake/gdbserver.config

#
# We need to copy the executable over to the target so that
# gdbserver can access it. The cksum will be empty if the file
# doesn't exist. We need to redirst stderr so the error for
# missing file doesn't get treated like the cksum value.
#
target_executable_cksum=`ssh $target_user@$target_ip "cksum $target_command 2>/dev/null" | awk '{print($1)}'`
build_executable_cksum=`cksum $build_command 2>/dev/null | awk '{print($1)}'`
    
if [ -z  $build_executable_cksum ]
then
    echo No build executable to debug
    exit 1
fi

#
# If the cksums are different then we need to copy the executable to the target
#
if [ $build_executable_cksum != $target_executable_cksum ]
then
    #
    # If the cksums don't match copy the new build command to the target
    #
    echo Copying new build command to target
    scp $build_command $target_user@$target_ip:$target_command 1>/dev/null 2>&1
else
    echo Using existing build command on target
fi

#
# Now that the build executable is on the target we can start the gdbserver
#
gdbserver_command="gdbserver $target_gdbserver_port $target_command"

#
# Check if the gdb server is running
#
gdb_server_pid=`ssh $target_user@$target_ip pgrep -x gdbserver`

#
# If it is running kill it and start a new one
#
if [ ! -z $gdb_server_pid ]
then
    echo Killing existing gdbserver
    ssh $target_user@$target_ip kill $gdb_server_pid
fi

echo Starting gdb server
ssh $target_user@$target_ip "nohup $gdbserver_command 0>/dev/null 1>&0 2>&0 &"

exit 0


