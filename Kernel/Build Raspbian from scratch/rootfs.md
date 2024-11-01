In those steps, we will setup our own rootfs from Busybox.

With Busybox, we can build our minimal custom rootfs which runs on the Raspberry Pi hardware platform.

Step will include:
1. Setup and build the busybox Github repo
2. Create a install script to put all the material in rootfs partition.

# 1. Build all Raspbian kernel modules inside the Raspbian linux repo
```sh
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules_install INSTALL_MOD_PATH=.
```
Before running this ``make`` command, there is no folder ``modules`` created inside ``lib`` folder. After running this make command, folder ``modules``, which include all kernel modules of the current Raspbian version, is created inside the ``lib`` folder of the Raspbian ``linux`` repo. That's why we need to set ``INSTALL_MOD_PATH=.``.

# 2. Setup and build busybox repository

- First clone the busybox repo from master branch
```sh
git clone https://github.com/mirror/busybox
```
- Inside the busybox repo, clean up any existed config (if running those steps for the 2nd times):

```sh
$ make clean
$ rm -f .config # Remove .config file if it has been created
```

Then specify the build configuration for busybox
```sh
make menuconfig
```
- Go to ``settings->Build static binary (no shared libs) (NEW)``, press ``y`` to select the option

- Then, build the busybox:
```sh
make -j$(nproc) ARCH="arm64" LDFLAGS="--static" CROSS_COMPILE="aarch64-linux-gnu-"
```
Once done, run this command to install the material in folder ``_install``
```sh
sudo make -j$(nproc) ARCH="arm64" LDFLAGS="--static" CROSS_COMPILE="aarch64-linux-gnu-" install
```

After building successfully, folder ``_install`` will be created inside the busybox repo.
```sh
username@hostname:~/raspbian_booting/busybox$ ls _install/
bin  linuxrc  sbin  usr
```

# 3. Create an install script to put all the material in rootfs partition

When kernel start to load the rootfs, it will try to find a starting point to load up services that needed for linux. There are several init methods that used in linux: ``SysV``, ``systemd``, ``openRC``, ...

For busybox, they no longer support [Systemd](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/Systemd.md) init which is a widely used init mechanism although it's a quite controlversial topic in the linux community whether to use it or not. However, busybox natively supports ``SysV`` init and that is what we're using today.

[Refer to SysV init specification](https://docs.oracle.com/cd/E19683-01/817-3814/6mjcp0qge/index.html) for more details on run level and file structure. This tutorial just go through some basic implementation so that we could have a functional linux terminal when boot into OS.

Our ``Raspbian_booting`` folder tree so far:

```sh
tranphucvinh@tranphucvinh:~/Documents/Gitlab/Raspbian_booting$ ls
bootfs  busybox  linux  rootfs  u-boot
```
## mdev
**mdev** is a command that is part of the BusyBox suite for controlling ownership/permissions of device nodes. It can indeed be used for controlling ownership, permissions, and other attributes of device nodes, especially in systems that need something more minimalistic than ``udev``. As being part of the Busybox, ``mdev`` isn't available in Ubuntu, Debian,...  On most systems, mdev is invoked during system boot to create the initial device node. This can be achived by ``mdev -s`` commands.

``mdev -s`` will scan ``/sys`` (``/sys/class`` and ``/sys/block``) to create ``/dev``. Base on [its implementation in Busybox](https://coral.googlesource.com/busybox/+/refs/tags/1_18_2/util-linux/mdev.c), ``mdev -s`` scans ``/sys/class/xxx``, looking for directories which have ``dev`` file, e.g ``/sys/class/tty/tty0/dev``. Then ``mdev`` creates the ``/dev/device_name`` node.

So a typical code snippet from the ``rcS`` init script will be:
```sh
mount -t proc proc /proc
mount -t sysfs sysfs /sys
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s
```

## Init script
Let's create the following ``rootfs_install.sh`` script inside ``busybox`` folder. Based on the ``Raspbian_booting`` folder tree, we can set the path for ``rootfs`` and ``kernel_modules_path``

```sh
$ cat rootfs_install.sh
#!/bin/sh
set -e
rootfs=../rootfs #path to your rootfs folder
kernel_modules_path=../linux
cp -r _install/* $rootfs/

mkdir $rootfs/etc
mkdir $rootfs/proc
mkdir $rootfs/sys
mkdir $rootfs/dev

mkdir $rootfs/etc/init.d
mkdir $rootfs/lib

echo '#!/bin/sh' > rcS

echo 'mount -t proc none /proc' >> rcS

echo 'mount -o remount,rw /' >> rcS

echo 'mount -t sysfs none /sys' >> rcS

echo 'echo /sbin/mdev > /proc/sys/kernel/hotplug' >> rcS

echo 'mdev -s' >> rcS

cp rcS $rootfs/etc/init.d/
chmod +x $rootfs/etc/init.d/rcS

cp -r $kernel_modules_path/lib/modules $rootfs/lib/

rm -f rcS
```

Finally, run
```sh
sudo ./rootfs_install.sh
```
Our final ``rootfs`` folder will include:

```sh
username@hostname:~/Raspbian_booting/rootfs$ ls
bin  dev  etc  lib  linuxrc  proc  sbin  sys  usr
```
Finally, copy that rootfs folder to the rootfs partition of the SD card. Along with the [bootfs partition previously setup](bootfs.md), Raspberry Pi board now is able to be **booted by the customized Raspbian on that SD card by USB UART** baudrate 115200, not HDMI.

With the customized Linux OS booted successfuly, [rcS](https://github.com/TranPhucVinh/Linux-Shell/tree/master/Physical%20layer/Init%20script#rcs) file now is:
```sh
#!/bin/sh
mount -t proc none /proc
mount -o remount,rw /
mount -t sysfs none /sys
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s
```
