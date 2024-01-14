A storage device, like SD card, which stores a customized Linux OS for Raspberry Pi includes 2 filesystem: [bootfs](bootfs.md) and [rootfs](rootfs.md).

Steps to build that Linux OS will include:

1. Raspbian kernel image and device tree built from [Official Raspberry Linux repository, branch rpi-5.15.y for Linux 5.15](https://github.com/raspberrypi/linux)
2. Booting from U-boot (instead of using Raspberry proprietary bootloader)
3. Create rootfs partition by using busybox.

# Essential packages

- Download these tools that necessary for build process later
```sh
sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev
sudo apt install crossbuild-essential-armhf
sudo apt install crossbuild-essential-arm64
```
- Connect the USB-UART TTL device to Raspbian and open a terminal port for interacting with boot process later on.

# Format bootfs and rootfs
First of all, create a folder to perform the whole booting process setup, like ``Raspbian_booting``. Inside ``Raspbian_booting``, create 2 folder ``bootfs`` and ``rootfs`` for the creation of 2 file systems of the same name.

Use [fdisk](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/File%20system/fdisk.md) to format [bootfs into FAT32 format with 100 MiB and rootfs into ext4 format which takes size of the leftover of the SD card](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/File%20system/fdisk.md#format-partitions-inside-a-sd-card).
# [Create bootfs file system](bootfs.md)

# [Create rootfs file system](rootfs.md)

# Create a built-in kernel module and load it into rootfs

Inside the Raspbian ``linux`` repository, go ``drivers`` folder then create ``raspbian_kernel_driver`` folder includes these files
```sh
raspbian_kernel_driver/
├── Kconfig
├── Makefile
└── raspbian_kernel_driver.c
```

**Kconfig**
```sh
config RPI_DRIVER
tristate "Raspbian kernel driver"
help
  Raspbian kernel driver
```
``tristate`` in Kconfig allows you to compile a feature as **built-in (y)**, **a module (m)** or **not at all (n)**.
**Makefile**
```sh
obj-$(CONFIG_RPI_DRIVER) += raspbian_kernel_driver.o
```
[raspbian_kernel_driver.c](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/raspbian_kernel_module.c)

Now we need to include this kernel module into the top level ``Makefile`` and ``Kconfig`` of the ``drivers`` folder

```sh
drivers/
├── Kconfig
├── Makefile
├── ...
```
Add the following line at the end of the ``Makefile``
```sh
obj-$(CONFIG_RPI_DRIVER) 		+= raspbian_kernel_driver/
```
For ``Kconfig``, add the following line, make sure it before the ``endmenu`` section
```sh
source "drivers/raspbian_kernel_driver/Kconfig"
```
Back to top-level of the Raspbian ``linux`` repo. As now, the whole kernel modules/driver has been updated, i.e adding the new raspbian_kernel_driver, new ``.config`` file has to be updated. Run ``make bcmrpi3_defconfig`` for that:

```sh
make bcmrpi3_defconfig ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
```

Then we need to include ``raspbian_kernel_driver`` into the module list by running ``make menuconfig``:

```sh
make menuconfig ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
```

Then go to ``Device Drivers`` -> scroll down and find option ``Raspbian kernel driver``

Press ``y`` will build the driver later, press ``m`` to build and include it into module install path later on. Here we choose ``m``

Now, rebuild the whole kernel module. This will take a while
```sh
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules
```
After that, we install modules into a specific path
```sh
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules_install INSTALL_MOD_PATH=<custom path>
```
Copy the folder ``modules`` to the /lib inside rootfs partition.

Once the system boot up completely, ``modprobe raspbian_kernel_driver`` to insert the module.