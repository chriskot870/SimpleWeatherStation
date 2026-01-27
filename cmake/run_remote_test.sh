#!/bin/bash
REMOTE_HOST="chrisk@qwtest.local"
REMOTE_PATH="/usr/local/qw/tests"
LOCAL_BINARY="/home/chrisk/Projects/RaspberryPi/WS/build/src/lib/qw/units/pressure/tests/units_pressure_test" # TestExecutable

# # Remove the first argument so that any additional arguments can be passed to the remote command

LOCAL_FILE_NAME=$(basename "${LOCAL_BINARY}")
# Transfer the binary
scp "${LOCAL_BINARY}" "${REMOTE_HOST}:${REMOTE_PATH}/${LOCAL_FILE_NAME}"

# Execute remotely and capture result
ssh "${REMOTE_HOST}" "cd ${REMOTE_PATH} && ./${LOCAL_FILE_NAME}"
exit $?