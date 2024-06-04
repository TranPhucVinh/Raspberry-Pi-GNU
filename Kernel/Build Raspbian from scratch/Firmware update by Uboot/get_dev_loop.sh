#!/bin/bash

# This script will get all mounted blocked devices and compare them with the 
# /dev/loop* devices. The /dev/loop* device which isn't blocked devices is the 
# unmounted loop device

# All mounted loop devices are blocked devices. List them out by lsblk
mounted_loop=$(lsblk | grep loop | cut -d " " -f 1)

all_dev_loop=$(ls /dev/loop* | grep -v control | cut -d "/" -f3)
for dev_loop in $all_dev_loop
do
    is_match=false
    for loop in $mounted_loop
    do
	if [[ $dev_loop == $loop ]]; then
	    is_match=true
	    break
	fi
    done
    if [[ $is_match == false ]]; then
        echo -n "/dev/$dev_loop"
	break
    fi
done
