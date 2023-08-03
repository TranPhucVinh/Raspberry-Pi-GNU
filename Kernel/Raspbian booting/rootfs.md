# Setup our own rootfs from busybox

## Introduction
With busybox, we can build our minimal custom rootfs which runs on the Raspberry Pi hardware platform.

Step will include:
1. Setup and build the busybox Github repo
2. Create a install script to put all the material in rootfs partition.

## 1. Build all Raspbian kernel module inside the  Raspbian linux repo
```sh
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules_install INSTALL_MOD_PATH=.
```
**Note** 

1. Before running this command, there is no folder ``modules`` created inside ``lib`` folder.
2. As the top-level Makefile of the Raspbian ``linux`` repository is intented to be run on the Raspbian, when running this command in the current working PC:

```sh
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules_install #INSTALL_MOD_PATH is not specified
```
It will try to create a folder ``modules`` in ``/lib/`` of the current working PC which will result in this error:

```
 mkdir: cannot create directory ‘/lib/modules/5.15.92-v8+’: Permission denied
```

After running this make command, folder modules, which include all kernel modules of the current Raspbian version, is created inside the ``lib`` folder of the Raspbian ``linux`` repo. That's why we need to set ``INSTALL_MOD_PATH=.``.

## 2. Setup and build busybox repository

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
$ make -j$(nproc) ARCH="arm64" LDFLAGS="--static" CROSS_COMPILE="aarch64-linux-gnu-"
```
Once done, run this command to install the material in folder ``_install``
```sh
$ sudo make -j$(nproc) ARCH="arm64" LDFLAGS="--static" CROSS_COMPILE="aarch64-linux-gnu-" install
```

After building successfully, folder ``_install`` will be created inside the busybox repo.
```sh
username@hostname:~/raspbian_booting/busybox$ ls _install/
bin  linuxrc  sbin  usr
```

## 3. Create an install script to put all the material in rootfs partition.

When kernel start to load the rootfs, it will try to find a starting point to load up services that needed for linux. There are several init methods that used in linux: ``SysV``, ``systemd``, ``openRC``, ...

For busybox, they no longer support [Systemd](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/Systemd.md) init which is a widely used init mechanism although it's a quite controlversial topic in the linux community whether to use it or not. However, busybox natively supports ``SysV`` init and that is what we're using today.

[Refer to SysV init specification](https://docs.oracle.com/cd/E19683-01/817-3814/6mjcp0qge/index.html) for more details on run level and file structure. This tutorial just go through some basic implementation so that we could have a functional linux terminal when boot into OS.

Our ``Raspbian_booting`` folder tree so far:

```sh
tranphucvinh@tranphucvinh:~/Documents/Gitlab/Raspbian_booting$ ls
bootfs  busybox  linux  rootfs  u-boot
```

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
Finally, copy that rootfs folder to the rootfs partition of the SD card. Along with the [bootfs parition previously setup](bootfs.md), Raspberry Pi board now is able to be booted by the customized Linux OS on that SD card.
