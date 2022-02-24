#!/usr/bin/env bash

PICO_MOUNT_PATH="/media/bassam/RPI-RP2/"

echo "Waiting for Pico to be mounted in $PICO_MOUNT_PATH"
while true; do
  if [[ -d "$PICO_MOUNT_PATH" ]]; then
    echo "FOUND!"
    ./build.sh
  fi
  sleep 2s
done
