# Setup communication between host PC and the Raspberry Pi board containing this customized Raspbian then run a.out file

Prerequisites:
* Raspberry Pi board and host PC connect to the same network, we will use [TFTP](https://github.com/TranPhucVinh/Linux-Shell/tree/master/Application%20layer#tftp) for file transferring between them.
* [TFTP server setup on host PC](https://github.com/TranPhucVinh/Linux-Shell/tree/master/Application%20layer#tftp)

As this customized Raspbian doesn't have IP setup by default, we need to setup its IP to have the same subnet with the host PC for TFTP file transferring. Use [ifconfig]():
```sh
ifconfig eth0 192.168.1.15 netmask 255.255.255.0 broadcast 192.168.1.255
```
In this case, IP ``192.168.1.15`` of this customized Raspbian will have the same subnet with the host PC, e.g ``192.168.1.13``

Then assign IP ``192.168.1.15`` to eth0 interface permanently then add it to the routing table by ``route`` command:

```sh
# ifconfig eth0 up
# route add -net 192.168.1.0/24 eth0
```

Then compile ``main.c``, a C source code, in host PC by **aarch64-linux-gnu-gcc** compiler (using [-static](https://github.com/TranPhucVinh/C/blob/master/Environment/GCC%20compiler.md#-static) for [static library](https://github.com/TranPhucVinh/C/blob/master/Environment/Static%20library.md))

```sh
aarch64-linux-gnu-gcc -static main.c
```
With TFTP is well setup on host PC, copy the newly built ``a.out`` into ``/var/lib/tftpboot`` for customized Raspbian to get it via TFTP later:

```sh
sudo cp a.out /var/lib/tftpboot
```

In the customized Raspbian board, get that a.out file via TFTP:

```sh
tftp -g -r a.out 192.168.1.13
```
Then file ``a.out`` will be able to run on the customized Raspbian.
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
