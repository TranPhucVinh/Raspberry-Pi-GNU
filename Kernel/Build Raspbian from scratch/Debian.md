Debian distribution provides us base minimal rootfs so that we can customize and combine it with the kernel of our own

This tutorial will create a Debian rootfs.

Install some tools
```sh
sudo apt install qemu-user-static debootstrap -y
```
* **debootstrap**: debootstrap tool was provided by the Debian community for the installation of Debian rootfs. It's the most convenient way to create a rootfs without having to extract each installation packages from the upstream repository.
* **qemu-user-static**: enable an execution of different multi-architecture containers by QEMU and **binfmt_misc**. 

Step 1: In your workspace, create a ``debianfs`` folder
```sh
sudo mkdir debianfs
```

Then fetch the rootfs from a public Debian ftp server, here we use the **buster** release version because it's the lightest version of all
```sh
sudo debootstrap --arch=arm64 buster debianfs http://ftp.uk.debian.org/debian
```

Once it's done, then copy the qemu bin to debianfs
```sh
sudo cp /usr/bin/qemu-aarch64-static debianfs/usr/bin/
```

Next we setup the name server for DNS lookup which support the networking services, we can copy the current configuration from host machine to the rootfs
```sh
sudo cp /etc/resolv.conf debianfs/etc/
```
Create ``debianfs/etc/network/interfaces``

Adding these line in ``debianfs/etc/network/interfaces`` so that Debian can setup DHCP IP for the board, you should use sudo to edit the file
```sh
auto lo
iface lo inet loopback
auto eth0
iface eth0 inet dhcp
```
```eth0``` can be replaced for other ethernet device in other Raspberry Pi system (check your ethernet device with ```ip a``` command)

Create ``debianfs/etc/apt/sources.list``.

Then add some default repositories for package fetching in ```debianfs/etc/apt/sources.list```, once again you should edit it with sudo permission.
```sh
deb http://deb.debian.org/debian buster main contrib non-free
deb-src http://deb.debian.org/debian buster main contrib non-free
deb http://security.debian.org/ buster/updates main contrib non-free
deb-src http://security.debian.org/ buster/updates main contrib non-free
deb http://deb.debian.org/debian buster-updates main contrib non-free
deb-src http://deb.debian.org/debian buster-updates main contrib non-free
```

Now let switch root inside the debianfs to handle some extra stuff
```sh
sudo chroot debianfs
```
**Note**: If ``debootstrap`` isn't installed properly, there will be error with this chroot command

```sh
chroot: failed to run command ‘/bin/sh’: No such file or directory
```

Let's do some initial actions:
```sh
/# export LANG=C
/# apt install locales dialog
```

Then we configure locales
```sh
dpkg-reconfigure locales
```
Scroll down and look for ```en_US.UTF-8 UTF-8```, press ```space``` to select the option, then press ```tab``` and select OK.

We then installed some essential packages
```sh
/# apt install nano openssh-server sudo udev wget -y
```

Next change the root account password, set the password to be ``debian``:
```sh
/# passwd
New password:
Retype new password:
passwd: password updated successfully
```
The username by default is ``root``.

Then change the hostname of your OS, e.g change it to ```debian```
```sh
/# echo debian > /etc/hostname
```

Since we installed the openssh-server, if we want to allow access to root through ssh, we need to edit the file ```/etc/ssh/sshd_config```, uncomment this line and change its value to yes
```sh
PermitRootLogin yes
```
The last step is important, we need to edit the ```/etc/fstab``` like this for mounting the root file system
```sh
/# cat /etc/fstab
proc	/proc	proc	defaults	0	0
/dev/mmcblk0p1  /boot   vfat   defaults   0   0
/dev/mmcblk0p2  /       ext4   defaults   0   1
```

Then finish the process by exiting the switch root fs
```sh
/# exit
```

Last step is to copy [all kernel modules built from the Raspbian linux repo](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Build%20Raspbian%20from%20scratch/bootfs.md#build-the-kernel-image-device-tree-and-kernel-modules) into ``/lib/`` of ``debianfs``:

```sh
sudo cp -r <path_to_your_external_build_modules> debianfs/lib/
```
E.g:
```sh
username@hostname:~/Raspbian_booting$ sudo cp -r rootfs/lib/modules/ debianfs/lib/
```

And finally, we have a fully functional rootfs. Copy it to the SD card, along with the previously setup bootfs:
```sh
$ ls debian_fs
bin  boot  dev  etc  home  lib  media  mnt  opt  proc  root  run  sbin  srv  sys  tmp  usr  var
```
