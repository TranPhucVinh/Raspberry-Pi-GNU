**U-Boot** (fullname Das U-Boot) is the bootloader used in embedded devices, as the second stage bootloader, to boot the device's operating system kernel. It is available for various computer architectures, like ARM, RISC-V, x86,...

U-Boot is the standard bootloader for ARM Linux so it is also called ARM U-Boot.

In order to boot the kernel image, uboot need to know your hardware information, which requires those steps and files in Uboot developing:
* Create the board file
* Create the board Kconfig file
* Create the board Makefile
* Create the board defconfig
* Create the board header file
* Source board’s Kconfig in the architecture’s Kconfig
* Define the TARGET Kconfig option in its CPU’s Kconfig

The most difficult thing here is how to understand the board hardware and describe it into these file. Especially the default config file.

**u-boot repo supports popular boards config** (included in u-boot source code), e.g Raspbian. In this case when using Raspberry 3B+ board, the Uboot config file is [rpi_3_b_plus_defconfig](https://github.com/u-boot/u-boot/blob/master/configs/rpi_3_b_plus_defconfig).

For all commands of Uboot, check [uboot.txt](uboot.txt) file.
# bootargs
``bootargs`` environment parameter is used to pass command line parameter. It'll take those variables:
* ``root``, e.g ``root=/dev/mmcblk0p1``: Specify the root filesystem. This is required unless a RAM disk is specified in the U-Boot bootm command.
* ``rootwait`` (take no value): This flag should always be present if a ``root=<value>`` is specified. It tells the kernel to wait for the device to appear before trying to mount it.
* ``console``, e.g ``console=ttymxc1,115200``: Specify the Linux kernel console device (to which kernel messages are routed).
* ``init``, e,g ``init=/init``: After mounting the root filesystem, the Linux kernel runs a single program to start the system. This is typically either ``/init`` or ``/linuxrc``. You can override the default behavior if your userspace requires it by using the ``init=<parameter>``. **Note that this is rarely needed**.

* ``8250.nr_uarts``: ``8250`` is the 8250 UART IC, in which 8250 UART kernel configuration parameters can be like:
	* CONFIG_SERIAL_8250_RUNTIME_UARTS=4 
	* CONFIG_SERIAL_8250_NR_UARTS=4 

# fatlooad

``fatload`` loads binary file from a dos filesystem

```sh
fatload <interface> [<dev[:part]> [<addr> [<filename>]]]
```

This command will load binary file ``filename`` from ``dev`` on ``interface`` to address ``addr`` from dos filesystem.

# booti

booti - boot Linux kernel 'Image' format from memory

```sh
booti [addr [initrd[:size]] [fdt]]
```
- boot Linux flat or compressed 'Image' stored at 'addr'

The argument 'initrd' is optional and specifies the address of an initrd in memory. 

The optional parameter ':size' allows specifying the size of a RAW initrd.

Since booting a Linux kernel requires a flat device-tree, a third argument providing the address of the device-tree blob is required. To boot a kernel with a device-tree blob but without an initrd image, use a ``-`` for the initrd argument.
# mkimage

```sh
mkimage [-options] image  
```
* ``-A``: architecture
* ``-O``: operating system
* ``-T``: image type
* ``-C``: compression type
* ``-d``: specify the image data file

# ext4

``ext4ls``: list files in a directory (default ``/``) of an ext4 file system

```sh
U-Boot> ext4ls mmc 0:2 # This will list out all files inside the rootfs folder
<DIR>       4096 .
<DIR>       4096 ..
<DIR>       4096 bin
<DIR>       4096 dev
<DIR>       4096 etc
<DIR>       4096 lib
<SYM>         11 linuxrc
...
```

Load file ``base_busybox.img`` to RAM address at ``${loadaddr}`` on the a Ext4 filesystem

```sh
U-Boot> ext4load mmc 0:2 ${loadaddr} base_busybox.img  
157286400 bytes read in 6506 ms (23.1 MiB/s)
```

**${loadaddr}** is a U-Boot environment variable that represents the default memory address where U-Boot loads and executes images, such as the kernel, device tree, or ramdisk.

The **${loadaddr}** variable is commonly used in U-Boot commands and scripts to specify the memory address where the loaded file should be placed.