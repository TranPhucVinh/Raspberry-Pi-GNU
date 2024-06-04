# Overview

As Uboot can connect to the local network, via Ethernet by default in Raspbian, and supports DHCP and TFTP, we can use this feature to flash the new OS image to the SD card. However, some Uboot on Raspbian hardware has the issue of not able to boot the Ethernet interface, while the rootfs can boot it. But we can still manage to perform the firmware update by transfering the image file from rootfs to the Uboot console.

**Prerequisite**

* Current Raspberry board runs an OS image where its ``bootfs`` has a U-boot image
* TFTP server installed on the host PC to perform FW Update

# Step 1: Create the OS image on the host PC

First of all, create a folder to perform the Firmware update setup, like ``fw_update``. Inside ``fw_update``, setup 2 folder ``bootfs`` and ``rootfs`` for the 2 file systems of the same name to be updated to the SD card on Raspberry.

Create **base_busybox.img**, an empty OS image with the size of 150 MB to store the bootfs and rootfs in the later steps:

```sh
username@hostname:~/fw_update$ sudo dd if=/dev/zero of=base_busybox.img bs=1M count=150
```

Create the FAT32 and ext4 partitions for ``bootfs`` (size 50 MB) and ``rootfs`` (the rest of **base_busybox.img**) by fdisk command:

```sh
username@hostname:~/fw_update$ sudo fdisk base_busybox.img

Welcome to fdisk (util-linux 2.31.1).
Changes will remain in memory only, until you decide to write them.
Be careful before using the write command.

Device does not contain a recognized partition table.
Created a new DOS disklabel with disk identifier 0xa4677ff4.

Command (m for help): n
Partition type
   p   primary (0 primary, 0 extended, 4 free)
   e   extended (container for logical partitions)
Select (default p): p
Partition number (1-4, default 1): 
First sector (2048-307199, default 2048): 
Last sector, +sectors or +size{K,M,G,T,P} (2048-307199, default 307199): +50M

Created a new partition 1 of type 'Linux' and of size 50 MiB.

Command (m for help): t 
Selected partition 1
Hex code (type L to list all codes): c
Changed type of partition 'Linux' to 'W95 FAT32 (LBA)'.

Command (m for help): n
Partition type
   p   primary (1 primary, 0 extended, 3 free)
   e   extended (container for logical partitions)
Select (default p): e
Partition number (2-4, default 2): 
First sector (104448-307199, default 104448): 
Last sector, +sectors or +size{K,M,G,T,P} (104448-307199, default 307199): 

Created a new partition 2 of type 'Extended' and of size 99 MiB.

Command (m for help): t
Partition number (1,2, default 2): 
Hex code (type L to list all codes): 83

Changed type of partition 'Extended' to 'Linux'.

Command (m for help): p
Disk base_busybox.img: 150 MiB, 157286400 bytes, 307200 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0xa4677ff4

Device            Boot  Start    End Sectors Size Id Type
base_busybox.img1        2048 104447  102400  50M  c W95 FAT32 (LBA)
base_busybox.img2      104448 307199  202752  99M 83 Linux

Command (m for help): w
The partition table has been altered.
Syncing disks.
```
# Step 2: Format the image

Inside the ``fw_update`` folder, use the following script:
- [get_dev_loop.sh](get_dev_loop.sh): get the umounted loop device. We will use this device as a mount point for each image partition
- [format_partition.sh](format_partition.sh): Format each partitions of the image, fat32 for ``bootfs`` and ext4 for ``rootfs``
- [write_image_content.sh](write_image_content.sh): Write ``rootfs`` and ``bootfs`` content to the targeted image

```sh
username@hostname:~/fw_update$ ls
base_busybox.img  get_dev_loop.sh  rootfs bootfs
format_partition.sh write_image_content.sh
```

After we succeed creating the ``base_busybox.img``, we excecute the above script as below.

```sh
$ sudo ./format_partition.sh <path_to_base_busybox_image_file> $(./get_dev_loop.sh)
```

```sh
username@hostname:~/fw_update$ sudo ./format_partition.sh base_busybox.img $(./get_dev_loop.sh)
mkfs.fat 4.1 (2017-01-24)
mke2fs 1.44.1 (24-Mar-2018)
Discarding device blocks: done                            
Creating filesystem with 101376 1k blocks and 25376 inodes
Filesystem UUID: 55eb17ac-6c01-40ed-88a1-14d805827cb3
Superblock backups stored on blocks: 
	8193, 24577, 40961, 57345, 73729

Allocating group tables: done                            
Writing inode tables: done                            
Creating journal (4096 blocks): done
Writing superblocks and filesystem accounting information: done
```
The above script will format the image, it takes two parameters. The first is the path to the previously created ``base_busybox.img`` file, the second is loop device which is fetched by the script ``get_dev_loop.sh``

Now we perform the write content action using ``write_image_content.sh`` script. In order to run ``write_image_content.sh``, the owner and group of the bootfs must be ``root``:
```sh
username@hostname:~/fw_update$ sudo chown -R root bootfs/*
username@hostname:~/fw_update$ sudo chgrp root bootfs/*
```
Then run ``write_image_content``:
```sh
$ sudo ./write_image_content.sh <path_to_base_busybox_image_file> <path_to_bootfs_directory> <path_to_rootfs_directory>
```

```sh
username@hostname:~/fw_update$ sudo ./write_image_content.sh base_busybox.img bootfs rootfs
```
Once done, you got the complete ``base_busybox.img`` that can flash on the raspberry Pi. 

Finally, copy this image to the tftp server in order to be copied by the Uboot as the TFTP client in the later step:

```sh
username@hostname:~/fw_update$ sudo cp base_busybox.img /var/lib/tftpboot/
```