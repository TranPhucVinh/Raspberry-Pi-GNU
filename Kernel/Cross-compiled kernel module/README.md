This tutorial is intend to build a Raspbian kernel module from Ubuntu PC,

The general idea of this is to create a docker on that Ubuntu PC which has the environment of Raspberry Pi board wished to insert the kernel module on.

# Document folder structure

```sh
├── on_raspi
│   └── get_infos.sh
└── on_ubuntu
│    ├── Dockerfile
│    ├── run.sh
│    └── util
│        ├── artifacts.tar.gz
│        ├── build.sh
│        └── Makefile
└── src
    ├── main.c
```

All the files is put into two seperated folder **on_raspi** and **on_ubuntu**. As the name implies, the material in **on_raspi** folder must be executed on the Raspberry Pi and vice versa for **on_ubuntu** folder. It's not specific only for Ubuntu so it could be on any other host platforms like other Unix-like as Docker is installed with a suitable version.

``src`` is a folder to store kernel module source code, like ``main.c`` in this example.

# Step 1. Get the Raspberry Pi board configuration parameters

Run the script [get_infos.sh](on_raspi/get_infos.sh) with sudo privilege on **Raspberry Pi** which is wished to insert the kernel module on. The output is a [artifacts.tar.gz]() file, which is outputed inside the ``on_raspi`` folder. Save this file somewhere else for the later use.

File ``artifacts.tar.gz`` stores:
* Hash value of current git repo built the current Raspbian
* Current Raspbian repo config params

# Step 2. Build the Docker image on host machine

- First and the most crucial step is to put the **artifacts.tar.gz** generated in step 1 before inside the [util]() folder of [on_ubuntu]().
- After that, run the script [run.sh]() with 1 parameter which is the **name** of the docker that wished to create, e.g ``docker_for_raspbian_cross_compiler``. Wait for the whole process to finish which takes arround 30 minutes depends on the host machine.

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