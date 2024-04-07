# View [bootfs](bootfs.md) partition by mounting

```sh
~ # fdisk -l
Device       Boot StartCHS    EndCHS        StartLBA     EndLBA    Sectors  Size Id Type
/dev/mmcblk0p1    64,0,1      1023,3,32         8192     53i2479     524288  256M  c Win95 FAT32 (LBA) # This is the bootfs partition
/dev/mmcblk0p2    1023,3,32   1023,63,32      532480   31116287   30583808 14.5G 83 Linux
~ # mkdir /mnt
~ # mkdir /mnt/boot
~ # mount /dev/mmcblk0p1 /mnt/boot/
~ # ls /mnt/boot/
Image                      config.txt "System Volume Information"  fixup.dat
bcm2710-rpi-3-b.dtb        start.elf  boot.scr                     u-boot.bin bootcode.bin
```

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
