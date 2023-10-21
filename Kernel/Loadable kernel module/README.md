# LKM built on Raspbian

By default, modules stored in ``/lib/modules`` has no ``Makefile`` to build.

For example ``uname -r`` has ``5.10.17-v7+``

``/lib/modules/5.10.17-v7+`` has:

```
kernel                     modules.builtin.bin      modules.order
modules.alias              modules.builtin.modinfo  modules.softdep
modules.alias.bin          modules.dep              modules.symbols
modules.builtin            modules.dep.bin          modules.symbols.bin
modules.builtin.alias.bin  modules.devname
```

(no ``build`` folder)

To build kernel module, user has to install Kernel header for Raspbian: ``sudo apt-get install raspberrypi-kernel-headers``

After installing, ``/lib/modules/5.10.17-v7+`` will have: (has ``build`` folder)

```
build   modules.alias      modules.builtin            modules.builtin.bin      modules.dep      modules.devname  modules.softdep  modules.symbols.bin
kernel  modules.alias.bin  modules.builtin.alias.bin  modules.builtin.modinfo  modules.dep.bin  modules.order    modules.symbols
```

``Makefile`` to build the kernel module:

```Makefile
obj-m += raspbian_kernel_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```    

[raspbian_kernel_module.c](raspbian_kernel_module.c): The very first kernel module built on Raspbian

For kernel module insert and remove, check [Kernel document in C](https://github.com/TranPhucVinh/C/tree/master/Kernel/Loadable%20kernel%20module) as their implementations are identical.

In 12th February 2022, with Raspbian installed from ``2021-10-30-raspios-bullseye-armhf.img``, the module will be ``5.10.63-v7+``. Before installing the kernel header for Raspbian, there won't be ``build`` folder inside ``/lib/modules/5.10.17-v7+``.
# [Cross-compiled LKM](Cross-compiled%20kernel%20module)

**Note**: ``make oldconfig`` in Raspbian ``linux`` repo will read the existing .config file that was used for an old kernel and prompts the user for options in the current kernel source that are not found in the file. This is useful when taking an existing configuration and moving it to a new kernel.
# [KConfig](https://github.com/TranPhucVinh/C/blob/master/Kernel/KBuild.md): configs kernel module which stores Raspbian configuration

To get the configuration of Raspbian, insert the ``configs`` kernel module.

```sh
sudo modprobe configs #Insert the configs kernel module
```

After successfully inserting this module, file ``config.gz``, which stores/zips file ``config`` (only 1 file), will be exported to ``/proc``.

File ``config`` stores the configuration flags (CONFIG_*) when building:
```
CONFIG_CC_VERSION_TEXT="arm-linux-gnueabihf-gcc-8 (Ubuntu/Linaro 8.4.0-3ubuntu1) 8.4.0"
CONFIG_CC_IS_GCC=y
CONFIG_GCC_VERSION=80400
...
```