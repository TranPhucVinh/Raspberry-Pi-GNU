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

To build kernel module, user has to install Kernel header for Raspberry Pi: ``sudo apt-get install raspberrypi-kernel-headers``

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

For kernel module insert and remove, check ``README.md`` in ``C/Kernel/`` as their implementations are identical.

### Examples

* ``raspbian_kernel_module.c``: The very first kernel module built on Raspbian