#!/bin/bash

target_ip="192.168.50.6"
target_gdbserver_port=":4711"
target_command="/usr/local/qw/tests/units_temperature.test"
target_stdin="/dev/null"
target_stdout="/var/tmp/units_temperature_test.out"
target_stderr="/var/tmp/units_temperature_test.err"
target_user="chrisk"
build_command="build/src/lib/qw/units/temperature/tests/units_temperature.test"

echo Copying new Test command to target
scp $build_command $target_user@$target_ip:$target_command 1>/dev/null 2>&1

#
# Check if the gdb server is running
#
gdb_server_pid=`ssh $target_user@$target_ip pgrep -x gdbserver`

if [ ! -z  $gdb_server_pid ] 
then
  echo GDB server already running pid: $gdb_server_pid
  echo Killing existing gdbserver
  ssh $target_user@$target_ip kill $gdb_server_pid
else
  echo GDB server not running
fi
echo Starting gdb 
gdbserver_command="gdbserver $target_gdbserver_port $target_command"
echo ssh $target_user@$target_ip "nohup $gdbserver_command 0<$target_stdin 1>$target_stdout 2>$target_stderr &"
ssh $target_user@$target_ip "nohup $gdbserver_command 0<$target_stdin 1>$target_stdout 2>$target_stderr &"

#echo Copying new Test command to target
#    scp $build_command $target_user@$target_ip:$target_command 1>/dev/null 2>&1

exit 0
