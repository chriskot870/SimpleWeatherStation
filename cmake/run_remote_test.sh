#!/bin/bash
REMOTE_HOST="chrisk@qwtest.local"
REMOTE_PATH="/usr/local/qw/tests"
LOCAL_BINARY="$1" # TestExecutable

# Transfer the binary
scp "${LOCAL_BINARY}" "${REMOTE_HOST}:${REMOTE_PATH}/"

# Execute remotely and capture result
ssh "${REMOTE_HOST}" "cd ${REMOTE_PATH} && ./${LOCAL_BINARY}"
exit $?