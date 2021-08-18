By default, modules stored in ``/lib/modules`` has no ``Makefile`` to build.

Example: 

``uname -r`` has ``5.10.17-v7+``

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