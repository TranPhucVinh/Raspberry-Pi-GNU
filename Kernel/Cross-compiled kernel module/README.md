# Build with the Raspbian [linux](https://github.com/raspberrypi/linux) repository which includes the current commit hash on the board

The general idea is to get the commit hash of the Raspbian [linux](https://github.com/raspberrypi/linux) repository, as the **kernel hash**, i.e the Raspbian [linux](https://github.com/raspberrypi/linux) repository "version", which is used to build the Raspbian image to be deployed on the board. Then clone the Raspbian [linux](https://github.com/raspberrypi/linux) repository at this commit to build the kernel module.

**Note**: When build the kernel module in the mismatch **kernel hash** Raspbian [linux](https://github.com/raspberrypi/linux) repository, there will be error **Invalid kernel format**.

## Step 1: Get the kernel hash

Access to the Raspberry Pi board and run the following command to get the firmware hash:
```sh
FIRMWARE_HASH=$(zgrep "* firmware as of" /usr/share/doc/raspberrypi-bootloader/changelog.Debian.gz | head -1 | awk '{ print $5 }')
echo $FIRMWARE_HASH
```

With that ``FIRMWARE_HASH`` value, go to ``https://raw.github.com/raspberrypi/firmware/<FIRMWARE_HASH>/extra/git_hash`` to get the commit hash of the Raspbian [linux](https://github.com/raspberrypi/linux) repository which is used to build the Raspbian image to be deployed on the board. This link will return only the kernel hash (the commit hash of the Raspbian [linux](https://github.com/raspberrypi/linux) repository, which is used to build the Raspbian image to be deployed on the board).

## Step 2: Clone the Raspbian [linux](https://github.com/raspberrypi/linux) repository at the previously achieved kernel hash

In order to save space and time, we just need to clone the Raspbian [linux](https://github.com/raspberrypi/linux) repository at only kernel hash we previously in step 1 from the Raspberry Pi. This can be achived by shallow clone the repository with ``--depth=1``:

```sh
$ mkdir raspbian_linux # make the directory to store the Raspbian linux repository
$ cd raspbian_linux/ # go to that directory
$ git init # then init .git in that dir
$ git remote add origin git@github.com:raspberrypi/linux.git #git remote add origin <url>
$ git checkout FETCH_HEAD
```
## Step 3: Get the current Raspberry board config params

[Insert the configs kernel module to get the current configuration of Raspbian](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/bcfe052d396373da2658d2fec8faaaefd4fe85e8/Kernel/README.md#kconfig-configs-kernel-module-which-stores-raspbian-configuration):
```sh
sudo modprobe configs #Insert the configs kernel module
```
Then SCP ``/proc/config.gz`` from Raspberry to the current working folder:
```sh
username@hostname:~/pwd$ scp pi@192.168.1.8:/proc/config.gz .
```
```sh
username@hostname:~/pwd$ gunzip config.gz
```
Then ``config`` will be available.

Move ``config`` into ``raspbian_linux`` then rename it ``.config``.
## Step 4: Build the whole raspbian_linux folder

```sh
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
```
## Step 5: Build the cross-compiled kernel module

Inside ``raspbian_linux``, at the top-level, create the folder for the cross-compiled kernel module which includes 2 files:
* ``main.c``: Kernel module source code
* ``Makefile``: Makefile to build that kernel module

```Makefile
obj-m += main.o
DIR := $(HOME)/pwd/raspbian_linux #Path to the top-level Makefile of raspbian_linux, which is used to build the kernel

all:
	make -C $(DIR) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
clean:
	make -C $(DIR) M=$(PWD) clean
```

Creating that kernel module folder at this location will take advantages of the top-level Makefile to build it.

Finally, build that kernel module and insert it into the Raspbian board.

# Build with docker which contains the Raspbian environment included the commit hash
The general idea is to create a docker on that Ubuntu PC which has the environment of Raspberry Pi board that wished to insert the kernel module on. This will simply based on the idea of [Build with the Raspbian linux repository which includes the current commit hash on the board]().

# Document folder structure

```sh
├── on_raspi
│   └── get_infos.sh
├── on_ubuntu
│    ├── Dockerfile
│    ├── run.sh
│    └── util
│        ├── artifacts.tar.gz
│        ├── build.sh
│        └── Makefile
└── src
    └── main.c
```

All the files is put into two seperated folder **on_raspi** and **on_ubuntu**. As the name implies, the material in **on_raspi** folder must be executed on the Raspberry Pi and vice versa for **on_ubuntu** folder. It's not specific only for Ubuntu so it could be on any other host platforms like other Unix-like as Docker is installed with a suitable version.

``src`` is a folder to store kernel module source code, like ``main.c`` in this example.

# Step 1. Get the Raspberry Pi board configuration parameters

Run the script [get_infos.sh](on_raspi/get_infos.sh) with sudo privilege on **Raspberry Pi** which is wished to insert the kernel module on. The output is a **artifacts.tar.gz** file, which is outputed inside the ``on_raspi`` folder. Save this file somewhere else for the later use.

File **artifacts.tar.gz** stores:
* Hash value of current git repo built the current Raspbian
* Current Raspberry board config params

# Step 2. Build the Docker image on host machine

- First and the most crucial step is to put the **artifacts.tar.gz** generated in step 1 before inside the [util](on_ubuntu/util) folder of [on_ubuntu](on_ubuntu).
- After that, run the script [run.sh](on_ubuntu/run.sh) with 1 parameter which is the **name** of the docker that wished to create, e.g ``docker_for_raspbian_cross_compiler``. Wait for the whole process to finish which takes arround 30 minutes depends on the host machine.

**run.sh** mains steps are:
* Cloning [raspberrypi/linux](https://github.com/raspberrypi/linux) repository, the kernel source tree for Raspberry Pi which provides kernel builds.
* Building a docker, by [Dockerfile](on_ubuntu/Dockerfile), from the [raspberrypi/linux](https://github.com/raspberrypi/linux) repository

Docker images before running run.sh

```
tranphucvinh@tranphucvinh:~$ sudo docker images
[sudo] password for tranphucvinh: 
REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
ubuntu              latest              2dc39ba059dc        6 months ago        77.8MB
hello-world         latest              feb5d9fea6a5        18 months ago       13.3kB
```

Docker images before running run.sh: ``./run.sh docker_for_raspbian_cross_compiler``

```
tranphucvinh@tranphucvinh:~$ sudo docker images
REPOSITORY                           TAG                 IMAGE ID            CREATED             SIZE
docker_for_raspbian_cross_compiler   latest              6f1c7b07248c        5 minutes ago       2.34GB
ubuntu                               18.04               3941d3b032a8        10 days ago         63.1MB
ubuntu                               latest              2dc39ba059dc        6 months ago        77.8MB
hello-world                          latest              feb5d9fea6a5        18 months ago       13.3kB
```
# Step 3. Build cross-compile kernel modules

Go to the folder where you keep your source code. Execute the below command

```sh
docker run --rm -v $(pwd):/src <Docker images name> <src file>
```

```sh
tranphucvinh@tranphucvinh:~/Documents/Gitlab/work-in-progress/Cross compiler/$ cd src
tranphucvinh@tranphucvinh:~/Documents/Gitlab/work-in-progress/Cross compiler/src$ sudo docker run --rm -v "$(pwd)":/src docker_for_raspbian_cross_compiler main.c
```

``"$(pwd)"``: For working folder including space

**Note**: Source code folder name, i.e ``src`` in this example must be specified in [build.sh](on_ubuntu/util/build.sh).
