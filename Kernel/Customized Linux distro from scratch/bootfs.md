# Build a customized Raspberry Pi kernel image and hardware device tree
## Clone the official Raspberry linux repository
- Clone the Raspberry kernel source tree inside the working directory ``Raspbian_booting``, we use the stable ``rpi-5.15.y`` branch.
```sh
git clone -b rpi-5.15.y --depth=1 https://github.com/raspberrypi/linux
```
## Specify the compiler/kernel-ver/build-configuration
Specify the compiler/kernel-ver/build-configuration by creating ``.config`` file.  ``.config`` is the file to tell which configuration, e.g compiler, kernel modules, kernel drivers,... needed to be included:
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

## Build the kernel image/device tree/device modules
```sh
make -j$(nproc) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image modules dtbs
```
Running this ``make`` will take a while.
**Note**: If not setting ``.config`` file in the step above, user has to manually choose Y/N for thousands of configuration like compiler, kernel modules, kernel drivers,... when running this ``make`` command.

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

Where ``Image`` is the generic Linux kernel binary image file.

Now copy those 2 files to the bootfs partition of the SD card (bootfs folder).
# Setup [Uboot](Uboot.md) for Raspbian
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
- Build the [uboot](Uboot.md) image
```sh
make -j$(nproc) ARCH=arm CROSS_COMPILE=aarch64-linux-gnu-
```
After successfully building:
* ``u-boot.bin`` will be created inside the previously cloned ``u-boot`` repository
* [mkimage](Uboot.md#mkimage) will be created inside ``u-boot/tools``
- Copy the ``u-boot.bin`` to the bootfs partition of the SD card.

- Inside the bootfs partition of the SD card (bootfs folder), create ``config.txt`` with the following content:
```sh
enable_uart=1
kernel=u-boot.bin
arm_64bit=1
device_tree=bcm2710-rpi-3-b.dtb
```
* ``enable_uart=1``: Enable the the uart to be used by ``u-boot.bin``
* ``kernel=u-boot.bin`` gives the kernel name to be loaded which is u-boot.bin in our case
* ``arm_64bit=1`` forces the kernel loading system to assume a 64-bit kernel, starts the processors up in 64-bit mode.
* ``device_tree`` specifies the name of the ``.dtb`` file to be loaded.

Now, the bootfs partition of the SD card (bootfs folder) will include all those files:

* ``Image``
* ``bcm2710-rpi-3-b.dtb``
* ``u-boot.bin``
* ``bootcode.bin``
* ``fixup.dat``
* ``start.elf``
* ``config.txt``

**rootfs partition of the SD card now is still empty**

If everything works fine, once you plug in the SD card and turn on the board, we can boot into u-boot. Access to its CLI by the USB/UART setup.
# Uboot script to boot the kernel image

After booting from u-boot, the Raspbian then needs to boot the kernel image and then rootfs.

We need to configure the u-boot so that it can pass the correct kernel commandline and device tree binary to kernel.

We use script ``boot_cmd.txt`` (inside ``u-boot/tools``) as the u-boot script with some predefine command

## Step 1: Load kernel image

At first, we need to call [fatload](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Raspbian%20booting/Uboot.md#fatlooad) to load the kernel image.

List available mmc devices by ``mmc list``:
```
U-Boot> mmc list
mmc@7e202000: 0 (SD)
mmc@7e300000: 1
U-Boot> 
```
From that we can see we have 2 MMC device, with MMC device 0 is our Raspbian SD card.

Then, we need to define which part of the MMC SD card which stores the bootfs partition. Use ``mmc part`` to list available partition on current MMC SD card
```
U-Boot> mmc part

Partition Map for MMC device 0  --   Partition Type: DOS

Part			Start 		Sector		Num 		Sectors	UUID		Type
  1				2048      	204800    	32dcba99-01	0b
  2				206848    	30909440  	32dcba99-02	83
```
From the previous step to set up the SD card partition for bootfs and rootfs, we can see that in the current MMC SD card, part 1 is the bootfs and part 2 is the rootfs.
## Step 2: pass kernel command line parameters by [bootargs](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Raspbian%20booting/Uboot.md#bootargs)
## Step 3: Start booting Linux kernel image by [booti](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Raspbian%20booting/Uboot.md#booti)
## Step 4: Finish the script

Finally, our boot_cmd.txt script inside ``u-boot/tools`` will be:
```sh
$ cat boot_cmd.txt 
fatload mmc 0:1 ${kernel_addr_r} Image
setenv bootargs "8250.nr_uarts=1 root=/dev/mmcblk0p2 rootwait console=ttyS0,115200n8"
booti ${kernel_addr_r} - ${fdt_addr}
```
Where:
* ``fatload mmc 0:1 \${kernel_addr_r} Image``: Load the kernel image from partition 1 (boot partition) into memory.
* [bootargs](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Raspbian%20booting/Uboot.md#bootargs)
* [booti](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Raspbian%20booting/Uboot.md#booti)

Then run [mkimage](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Raspbian%20booting/Uboot.md#mkimage) inside the u-boot repo location to output to ``boot.scr``:
```sh
vinhtran@hostname:/u-boot/tools:$ ./mkimage -A arm64 -O linux -T script -C none -d boot_cmd.txt boot.scr
```

- Copy the boot.scr to the bootfs partition, all files inside that partition folder now are:

```sh
username@hostname:~/Raspbian_booting/bootfs$ ls
bcm2710-rpi-3-b.dtb  bootcode.bin  boot.scr  fixup.dat  Image  start.elf  u-boot.bin
```
Finally, copy that bootfs folder to the bootfs partition of the SD card
