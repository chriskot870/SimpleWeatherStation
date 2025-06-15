#!/bin/bash
#
# This starts the gdbserver on the target machine
#
# Load the gdbserver data
#
#. cmake/gdbserver.config
. $1
#
# Make sure the executable got built
#
build_executable_cksum=`cksum $build_command 2>/dev/null | awk '{print($1)}'`
    
if [ -z  $build_executable_cksum ]
then
    echo No build executable to debug
    exit 1
fi

#
# Make sure the config file is available
#
build_config_cksum=`cksum $build_config 2>/dev/null | awk '{print($1)}'`
    
if [ -z  $build_config_cksum ]
then
    echo No build config file
    exit 1
fi

#
# We need to copy the executable over to the target so that
# gdbserver can access it. We check if the files exists on the
# target. If it does then we get the checksum. Otherwise, we
# set the checksum to zero so it won't match the newly built
# executable. That will force a copy.
#
ssh $target_user@$target_ip test -f $target_command
if [ $? -eq 0 ]
then
   echo Getting target cksum
   target_executable_cksum=`ssh $target_user@$target_ip "cksum $target_command 2>/dev/null" | awk '{print($1)}'`
else
  target_executable_cksum="0"
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
# We need to copy the config over to the target so that
# the executable can access it. We check if the file exists on the
# target. If it does then we get the checksum. Otherwise, we
# set the checksum to zero so it won't match the newly built
# executable. That will force a copy.
#
ssh $target_user@$target_ip test -f $target_config
if [ $? -eq 0 ]
then
   echo Getting target cksum
   target_config_cksum=`ssh $target_user@$target_ip "cksum $target_config 2>/dev/null" | awk '{print($1)}'`
else
  target_config_cksum="0"
fi

#
# If the cksums are different then we need to copy the config to the target
#
if [ $build_config_cksum != $target_config_cksum ]
then
    #
    # If the cksums don't match copy the new build config to the target
    #
    echo Copying new configuration to target
    scp $build_config $target_user@$target_ip:$target_config 1>/dev/null 2>&1
else
    echo Using existing configuration on target
fi

#
# Now that the build executable is on the target we can start the gdbserver
#
gdbserver_command="gdbserver $target_gdbserver_port $target_command $target_args"

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
ssh $target_user@$target_ip "nohup $gdbserver_command 0<$target_stdin 1>$target_stdout 2>$target_stderr &"

exit 0


