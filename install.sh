#!/bin/sh
set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 /dev/sdX" >&2
    exit 1
fi

DEVICE="$1"

if [ ! -b "$DEVICE" ]; then
    echo "Error: $DEVICE is not a block device" >&2
    exit 1
fi

make

sudo dd if=os-image.iso of="$DEVICE" bs=4M status=progress conv=fsync
sync

echo "Installed OS image to $DEVICE"

