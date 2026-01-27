#!/bin/bash


echo start_unit_test_gdb_server.sh $@

target_ip="192.168.50.6"
target_gdbserver_port=":4711"
target_command_dir="/usr/local/qw/tests"
target_stdin="/dev/null"
target_stdout="/var/tmp/units_temperature_test.out"
target_stderr="/var/tmp/units_temperature_test.err"
target_user="chrisk"
build_command=$1

target_executable=`basename $build_command`

target_stdin="/dev/null"
target_stdout="/var/tmp/$target_executable.out"
target_stderr="/var/tmp/$target_executable.err"

target_command="$target_command_dir/$target_executable"
echo Copying new Test command to target $target_command
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
