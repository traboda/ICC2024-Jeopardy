#!/bin/sh
TEMP_DIR=$(mktemp -d)
cp ./boot.bin "$TEMP_DIR/boot.bin"
cd "$TEMP_DIR"
qemu-system-i386 -nographic -drive file=boot.bin,format=raw -no-shutdown -monitor none
cd -
rm -rf "$TEMP_DIR"