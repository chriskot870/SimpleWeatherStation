#!/bin/bash

# Load the gdbserver data
#
#. cmake/gdbserver.config

build_command="build/src/WeatherStation"
target_user="chrisk"
target_ip="192.168.50.6"
target_command="/usr/local/qw/bin/WeatherStation"

scp $build_command $target_user@$target_ip:$target_command 1>/dev/null 2>&1

exit 0