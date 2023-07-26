# Features

Steps will include:

1. Raspbian kernel image and device tree built from [Official Raspberry Linux repository, branch rpi-5.15.y for Linux 5.15](https://github.com/raspberrypi/linux)
2. Booting from U-boot (instead of using Raspberry proprietary bootloader)
x. Create a bootable rootfs OS
x. Combine everything together and boot the RasPI to the completed OS

To create a bootable rootfs, we will use either of the tool like busybox.

# Essential packages

- Download these tools that necessary for build process later
```sh
sudo apt install git bc bison flex libssl-dev make libc6-dev libncurses5-dev
sudo apt install crossbuild-essential-armhf
sudo apt install crossbuild-essential-arm64
```
- Connect the USB-UART TTL device to Raspbian and open a terminal port for interracting with boot process later on.

# Format bootfs and rootfs
Use [fdisk](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/File%20system/fdisk.md) to format bootfs and rootfs partitions of SD card.
# Build a customize RasPI kernel image and hardware device tree

- First clone the Raspberry kernel repo, we use the stable ```rpi-5.15.y``` branch in this topic.
```sh
git clone -b rpi-5.15.y --depth=1 https://github.com/raspberrypi/linux
```
- Specify the compiler/kernel-ver/build-configuration
```sh
cd linux
make bcmrpi3_defconfig ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
```
Where target ``bcmrpi3_defconfig`` is defined by wildcard target ``%config`` of [Raspberry Pi Linux top-level Makefile](https://github.com/raspberrypi/linux/blob/rpi-6.1.y/Makefile)

```Makefile
include $(srctree)/scripts/Kbuild.include # This include is for $(build)

%config: outputmakefile scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@
```
``$(build)`` is defined in [scripts/Kbuild.include](https://github.com/raspberrypi/linux/blob/master/scripts/Kbuild.include#L164):

```Makefile
build := -f $(if $(KBUILD_SRC),$(srctree)/)scripts/Makefile.build obj
```
After running ``make bcmrpi3_defconfig ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-``, ``.config`` is created.

- Build the kernel image/device tree/device modules. It will take a while
```sh
make -j$(nproc) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image modules dtbs
```

Target ``Image`` is defined in [linux/arch/arm64/Makefile](https://github.com/raspberrypi/linux/blob/rpi-5.15.y/arch/arm64/Makefile)

```Makefile
Image: vmlinux
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@
```
Top-level Make has this line to include [linux/arch/arm64/Makefile](https://github.com/raspberrypi/linux/blob/rpi-5.15.y/arch/arm64/Makefile):

```Makefile
include $(srctree)/arch/$(SRCARCH)/Makefile
```

After running ``make -j$(nproc) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image modules dtbs``, ``Image`` (kernel image) and device tree file ``bcm2710-rpi-3-b.dtb`` will be available:
* ``Image`` will be available in ``arch/arm64/boot``
* ``bcm2710-rpi-3-b.dtb`` will be available in ``arch/arm64/boot/dts/broadcom/``
* 
Where ``Image`` is the generic Linux kernel binary image file.

Now copy those 2 files to the bootfs partition of the SD card.
# Setup Uboot for Raspbian
Raspberry Pi has its own proprietary bootloader, which is loaded by the ROM code and is capable of loading the kernel. This is what happen when you use the comercial Raspbian. It's worth mentioning from the [Raspbian booting process](Raspbian%20booting%20process.md) that in **step 4**, **start.elf** read **[config.txt](#configtxt)** to get the booting information like which kernel image file to load,... **start.elf** will then load the kernel image, like **kernel.img**. It then also reads **cmdline.txt** and **device tree binary files**.

However, since we would like to use the open source u-boot, we need to configure the Raspberry Pi boot loader to load u-boot and then let u-boot load the kernel. The booting step in step 4 now will be: **start.elf** loads **uboot**, **uboot** then loads **kernel.img**. The left steps are unchanged.

All the bootloader files of Raspbian are available in the [Raspberry Pi firmware Github repository](https://github.com/raspberrypi/firmware/tree/master/boot). We need to download those 3 files ``bootcode.bin``, ``fixup.dat`` and ``start.elf`` in the master branch. Copy all those 3 files to the bootfs partition of the SD card.

Our next step is to build the ``u-boot.bin`` (the binary compiled U-Boot bootloader):
- Clone the u-boot repo: 
```sh
git clone https://github.com/u-boot/u-boot --depth=1
```
- ``cd`` into the ``u-boot`` directory and specify the compiler/build configuration:
```sh
make rpi_3_b_plus_defconfig ARCH=arm CROSS_COMPILE=aarch64-linux-gnu-
```
- Setting the autoboot delay time in u-boot config to 10 seconds to easily enter the u-boot console if needed
```sh
make menuconfig ARCH=arm CROSS_COMPILE=aarch64-linux-gnu-
-> go to Boot options->Autoboot options->delay
```
- Build the uboot image
```sh
make -j$(nproc) ARCH=arm CROSS_COMPILE=aarch64-linux-gnu-
```
After successfully building:
* ``u-boot.bin`` will be created inside the previously cloned ``u-boot`` repository
* ``mkimage`` will be created inside ``u-boot/tools``
- Copy the ``u-boot.bin`` to the bootfs partition of the SD card.

- Inside the bootfs partition of the SD card, create ```config.txt``` with the following content:
```sh
enable_uart=1
kernel=u-boot.bin
arm_64bit=1
device_tree=bcm2710-rpi-3-b.dtb
```
Now, the bootfs partition of the SD card will include all those files:

* ``Image``
* ``bcm2710-rpi-3-b.dtb``
* ``u-boot.bin``
* ``bootcode.bin``
* ``fixup.dat``
* ``start.elf``
* ``config.txt``

**rootfs partition of the SD card now is still empty**

If everything works fine, once you plug in the SD card and turn on the board, we can boot into u-boot. Access to its CLI by the USB/UART setup.

For all commands of Uboot, check [uboot.txt](uboot.txt) file.
