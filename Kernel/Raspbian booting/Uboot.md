**U-Boot** (fullname Das U-Boot) is the bootloader used in embedded devices, as the second stage bootloader, to boot the device's operating system kernel. It is available for various computer architectures, like ARM, RISC-V, x86,...

U-Boot is the standard bootloader for ARM Linux so it is also called ARM U-Boot.

For all commands of Uboot, check [uboot.txt](uboot.txt) file.
# bootargs
``bootargs`` environment parameter is used to pass command line parameter. It'll take those variables:
* ``root``, e.g ``root=	/dev/mmcblk0p1``: Specify the root filesystem. This is required unless a RAM disk is specified in the U-Boot bootm command.
* ``rootwait`` (take no value): This flag should always be present if a ``root=<value>`` is specified. It tells the kernel to wait for the device to appear before trying to mount it.
* ``console``, e.g ``console=ttymxc1,115200``: Specify the Linux kernel console device (to which kernel messages are routed).
* ``init``, e,g ``init=	/init``: After mounting the root filesystem, the Linux kernel runs a single program to start the system. This is typically either ``/init`` or ``/linuxrc``. You can override the default behavior if your userspace requires it by using the ``init=<parameter>``. **Note that this is rarely needed**.

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

The argument 'initrd' is optional and specifies the address	of an initrd in memory. 

The optional parameter ':size' allows specifying the size of a RAW initrd.

Since booting a Linux kernel requires a flat device-tree, a third argument providing the address of the device-tree blob is required. To boot a kernel with a device-tree blob but without an initrd image, use a ``-`` for the initrd argument.