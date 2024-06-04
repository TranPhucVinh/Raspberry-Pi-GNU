#!/bin/bash

image_file=$1
boot_dir=$2
root_dir=$3
mkdir -p /mnt/image

p1_line=$(fdisk -l $image_file | tail -2 | head -1)
p2_line=$(fdisk -l $image_file | tail -1)
p1_start=$(echo -n "$p1_line" | tr -s " " | cut -d " " -f2)
p2_start=$(echo -n "$p2_line" | tr -s " " | cut -d " " -f2)

mount -o offset=$((p1_start*512)) $image_file /mnt/image
rm -rf /mnt/image/*
cp -a $boot_dir/* /mnt/image/
umount /mnt/image

mount -o offset=$((p2_start*512)) $image_file /mnt/image
rm -rf /mnt/image/*
cp -a $root_dir/* /mnt/image/
umount /mnt/image
