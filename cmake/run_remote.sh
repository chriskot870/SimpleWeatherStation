#!/usr/bin/env bash
set -e

TARGET_HOST="chris@qwtest.local"
TARGET_DIR="/tmp/cmake_test"
REMOTE_BINARY="$TARGET_DIR/$(basename "$1")"

# 1. Copy binary to remote
ssh "$TARGET_HOST" "mkdir -p $TARGET_DIR"
scp "$1" "$TARGET_HOST:$REMOTE_BINARY"

# 2. Shift script args so remaining are test arguments
shift

# 3. Run binary on remote and forward stdout/stderr
ssh "$TARGET_HOST" "$REMOTE_BINARY" "$@"

exit 0
