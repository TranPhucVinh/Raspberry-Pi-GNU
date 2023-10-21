#!/bin/bash

set -e

sudo modprobe configs #Insert configs kernel module

FIRMWARE_HASH=$(zgrep "* firmware as of" /usr/share/doc/raspberrypi-bootloader/changelog.Debian.gz | head -1 | awk '{ print $5 }')
KERNEL_HASH=$(wget https://raw.github.com/raspberrypi/firmware/$FIRMWARE_HASH/extra/git_hash -O -)

mkdir -p build_material
cd build_material

echo $KERNEL_HASH > hash.txt
cp /proc/config.gz ./

cd ../

tar -czvf artifacts.tar.gz ./build_material

chown $USER artifacts.tar.gz

rm -fr build_material