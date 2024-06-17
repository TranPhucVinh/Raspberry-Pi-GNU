# Build Raspbian from scratch

3 steps to build a customized Raspbian:

1. Raspbian kernel image and device tree built from [Official Raspberry Linux repository, branch rpi-5.15.y for Linux 5.15](https://github.com/raspberrypi/linux)
2. Booting from U-boot (instead of using Raspberry proprietary bootloader)
3. Create rootfs partition from **Busybox**.

After successfully taking those 3 steps, the Raspberry Pi board with the customized Raspbian can be **booted successfully from USB**, not HDMI (as there is no configuration to support HDMI in those steps). As being a **minimal Linux**, this Raspbian doesn't have SSH and netutils package like ping. All neccessary drivers like I2C, SPI are still be available, which require a little more bootfs and rootfs re-build steps.

As using busybox, TFTP will be supported on this customized Raspbian which will help transferring files.

## Prerequisites
A storage device, like SD card, which stores a customized Raspbian includes 2 filesystem: [bootfs](bootfs.md) and [rootfs](rootfs.md). So you need to [format bootfs and rootfs](#format-bootfs-and-rootfs) on that SD card. Then, you need to [install the essential packages](#install-essential-packages).

### Format bootfs and rootfs
First of all, create a folder to perform the whole booting process setup, like ``Raspbian_booting``. Inside ``Raspbian_booting``, create 2 folder ``bootfs`` and ``rootfs`` for the creation of 2 file systems of the same name.

Use [fdisk](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/File%20system/fdisk.md) to format [bootfs into FAT32 format with 100 MiB and rootfs into ext4 format which takes size of the leftover of the SD card](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/File%20system/fdisk.md#format-partitions-inside-a-sd-card).

### Install essential packages

- Download these tools that necessary for the later build process:
```sh
sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev
sudo apt install crossbuild-essential-armhf
sudo apt install crossbuild-essential-arm64
```
- Connect the USB-UART TTL device to Raspbian and open a terminal port for interacting with boot process later on.

## [Create bootfs file system](bootfs.md)
* [Build Raspbian kernel image and device tree](bootfs.md#build-raspbian-kernel-image-and-device-tree)
* [Setup Uboot for Raspbian](bootfs.md#setup-uboot-for-raspbian)

## [Create rootfs file system](rootfs.md)
* [Build all Raspbian kernel modules inside the Raspbian linux repo](rootfs.md#1-build-all-raspbian-kernel-modules-inside-the-raspbian-linux-repo)
* [Setup and build busybox]()
* [init with systemd]()

Along with the [bootfs partition previously setup](bootfs.md), Raspberry Pi board now is able to be **booted by the customized Raspbian on SD card by USB UART** baudrate 115200, not HDMI.
# [Implementations](Implementations.md)

Implementations on this customized Raspbian include:
* View bootfs partition by mounting
* Create a built-in kernel module and load it into rootfs
* Enable I2C and SPI devices
* [Networking](Networking)