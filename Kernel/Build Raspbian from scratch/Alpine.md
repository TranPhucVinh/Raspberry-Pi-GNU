Alpine distribution provides us base minimal rootfs so that we can customize and combine it with the kernel of our own
# Prerequisties

## SD card is formated with bootfs and rootfs

Like our prerequisties for the Raspberry Pi to run with Busybox, we need to format the SD card with [bootfs into FAT32 format with 100 MiB and rootfs for Alpine into ext4 format which takes size of the leftover](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/File%20system/fdisk.md#format-partitions-inside-a-sd-card).

## bootfs setup with Uboot

[bootfs folder](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Build%20Raspbian%20from%20scratch/bootfs.md) for the boot partion with Uboot is properly setup.

## QEMU
Working with the aarch64 Apline rootfs (for Raspbian) on a x86_64 PC, e.g Ubuntu PC, requires QEMU installed. ``qemu-user-static`` enables an execution of different multi-architecture containers by QEMU and binfmt_misc. 

Install ``qemu-user-static``: ``sudo apt install qemu-user-static -y``

After installing, all qemu package will be available inside ``/usr/bin``.
# Download and prepare scripts

Alpine is a light weight OS, which makes perfect sense for embedded system.

To download the Alpine rootfs, get to [its official download page](https://alpinelinux.org/downloads/), in ```MINI ROOT FILESYSTEM``` choose  **aarch64** version for Raspberry Pi. The downloaded file will be **alpine-minirootfs-3.20.0-aarch64.tar.gz**

Extract to ``alpinefs``:
```sh
$ mkdir alpinefs
$ tar -xzvf alpine-minirootfs-3.20.0-aarch64.tar.gz -C alpinefs
$ ls alpinefs/
bin  dev  etc  home  lib  media  mnt  opt  proc  root  run  sbin  srv  sys  tmp  usr  var
```

Next we setup the name server for DNS lookup which support the networking services for the [later steps to install the packages for Alpine](#install-alpine-packages).

We can copy the current configuration from host machine to the rootfs:
```sh
sudo cp /etc/resolv.conf alpinefs/etc/
```
Then create file ``interfaces`` inside ``alpinefs/etc/network/interfaces`` and add the following contents:
```sh
# adding these line to alpinefs/etc/network/interfaces
auto lo
iface lo inet loopback
auto eth0
iface eth0 inet dhcp
########################
```

Then change the hostname to alpine:
```sh
alpinefs $ echo alpine > etc/hostname
```
# Install Alpine packages

Copy the ``qemu-aarch64-static`` from the host PC to ``alpinefs/usr/bin/`` folder:
```sh
username@hostname:~/Raspbian_booting$ sudo cp /usr/bin/qemu-aarch64-static alpinefs/usr/bin/
```

Then switch root of the current subshell to ``alpinefs`` folder, then login to this root.
```sh
username@hostname:~/Raspbian_booting$ sudo chroot alpinefs /bin/sh -l
```
**Note**: If there is no ``qemu-aarch64-static`` inside ``alpinefs/usr/bin``, there will be error with this chroot command:

```sh
chroot: failed to run command ‘/bin/sh’: No such file or directory
```

After chroot and loging successfully, the CLI will be:
```sh
username@hostname:~/Raspbian_booting$ sudo chroot alpinefs /bin/sh -l
username:/# 
```

From now, you have already login into the rootfs of Alpine OS. You can start installing the neccessary packages for it. All packages in later steps will be installed into Alpinefs.

**apk** command is the package manager for Alpine Linux, which stands for **Alpine Package Keeper**. The apk command allows you to install, remove, update, and manage software packages. This is similar to ``apt-get update`` in Debian-based systems.

The **alpine-base** package includes essential components and utilities that form the base of an Alpine Linux installation. This package is a core part of the system and provides foundational elements needed for a minimal and functional Alpine environment.

The alpine-base package typically includes the following:

* BusyBox: A single binary that provides many common Unix utilities
* alpine-baselayout: Provides essential system configuration and layout files
* openrc: A dependency-based init system that manages services
* alpine-conf: Configuration scripts for initial system setup
* ssl: Provides SSL certificates

First install the alpine base
```sh
/# apk add --update alpine-base
```
# Setup init script
Set password for ``root`` account. The password like ``alpine`` is considered too weak.
```sh
/# passwd root
Changing password for root
New password:
Bad password: too short
Retype password:
passwd: password for root changed by root
```
Edit file ``/etc/inittab`` and uncomment this line
```sh
ttyS0::respawn:/sbin/getty -L ttyS0 115200 vt100
```
This allow us to login into the shell through serial port

The default ``/etc/fstab`` is:
```sh
/dev/cdrom	/media/cdrom	iso9660	noauto,ro 0 0
/dev/usbdisk	/media/usb	vfat	noauto,ro 0 0
```
This is how Alpine will be mounted by default. In our current configuration, append the following to ``/etc/fstab``:
```sh
/dev/mmcblk0p1  /boot   vfat   defaults   0   0
/dev/mmcblk0p2  /       ext4   defaults   0   1
```
Alpine uses ``OpenRC`` as its init system, by default some key services will not be enable, so we have to specify them by running those commands:
```sh
rc-update add devfs sysinit
rc-update add dmesg sysinit
rc-update add mdev sysinit
rc-update add hwdrivers sysinit

rc-update add modules boot
rc-update add sysctl boot
rc-update add hostname boot
rc-update add bootmisc boot
rc-update add syslog boot
rc-update add networking boot

rc-update add mount-ro shutdown
rc-update add killprocs shutdown
rc-update add savecache shutdown

rc-update add firstboot default
```
Exit the switch root
```sh
/# exit
```
Last step is to copy [all kernel modules built from the Raspbian linux repo](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Build%20Raspbian%20from%20scratch/bootfs.md#build-the-kernel-image-device-tree-and-kernel-modules) into ``/lib/`` of alpinefs:

```sh
sudo cp -r <path_to_your_external_build_modules> debianfs/lib/
```
E.g:
```sh
username@hostname:~/Raspbian_booting$ sudo cp -r rootfs/lib/modules/ alpinefs/lib/
```
Where ``rootfs`` is the [root file system built from Raspbian linux repo](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Build%20Raspbian%20from%20scratch/rootfs.md).

Now we have successfully configure a working rootfs for alpine. Copy both bootfs and the Alpine rootfs to SD card, Raspberry Pi will run the Alpine OS successfully.