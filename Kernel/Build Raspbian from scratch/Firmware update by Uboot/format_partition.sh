#!/bin/bash

image_file=$1
loop_dev=$2

p1_line=$(fdisk -l $image_file | tail -2 | head -1)
p2_line=$(fdisk -l $image_file | tail -1)
p1_start=$(echo -n "$p1_line" | tr -s " " | cut -d " " -f 2)
p2_start=$(echo -n "$p2_line" | tr -s " " | cut -d " " -f 2)

losetup -o $((p1_start*512)) $loop_dev $image_file
mkfs.vfat $loop_dev
losetup -d $loop_dev

losetup -o $((p2_start*512)) $loop_dev $image_file
mkfs.ext4 $loop_dev
losetup -d $loop_dev

: '
Result:

username@hostname:~/fw_update$ fdisk -l base_busybox.img | tail -2 | head -1
base_busybox.img1        2048 104447  102400  50M  c W95 FAT32 (LBA)

username@hostname:~/fw_update$ fdisk -l base_busybox.img | tail -1
base_busybox.img2      104448 307199  202752  99M 83 Linux

username@hostname:~/fw_update$ p1_line=$(sudo fdisk -l base_busybox.img | tail -2 | head -1)
username@hostname:~/fw_update$ p1_start=$(echo -n "$p1_line" | tr -s " " | cut -d " " -f 2)
username@hostname:~/fw_update$ echo $p1_start # This will get the size of base_busybox.img1 
2048

'