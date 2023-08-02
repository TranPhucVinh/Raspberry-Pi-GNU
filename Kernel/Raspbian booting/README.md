# Build a custom Linux OS for Raspberry Pi

A storage device, like SD card, which stores a customized Linux OS for Raspberry Pi includes 2 filesystem: [bootfs](bootfs.md) and [rootfs](rootfs.md).

Steps to build that Linux OS will include:

1. Raspbian kernel image and device tree built from [Official Raspberry Linux repository, branch rpi-5.15.y for Linux 5.15](https://github.com/raspberrypi/linux)
2. Booting from U-boot (instead of using Raspberry proprietary bootloader)
3. Create rootfs partition by using busybox.

## Essential packages

- Download these tools that necessary for build process later
```sh
sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev
sudo apt install crossbuild-essential-armhf
sudo apt install crossbuild-essential-arm64
```
- Connect the USB-UART TTL device to Raspbian and open a terminal port for interacting with boot process later on.

## Format bootfs and rootfs
First of all, create a folder to perform the whole booting process setup, like ``Raspbian_booting``. Inside ``Raspbian_booting``, create 2 folder ``bootfs`` and ``rootfs`` for the creation of 2 file systems of the same name.

Use [fdisk](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/File%20system/fdisk.md) to format bootfs and rootfs partitions of SD card.
## [Create bootfs file system](bootfs.md)

## [Create rootfs file system](rootfs.md)