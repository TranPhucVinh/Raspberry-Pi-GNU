# Loadable kernel module

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

For kernel module insert and remove, check ``README.md`` in ``C/Kernel/`` as their implementations are identical.

In 12th February 2022, with Raspbian installed from ``2021-10-30-raspios-bullseye-armhf.img``, the module will be ``5.10.63-v7+``. After installing the kernel header for Raspbian, there won't be ``build`` folder inside ``/lib/modules/5.10.17-v7+``.

**Examples**

* ``raspbian_kernel_module.c``: The very first kernel module built on Raspbian

# GPIO driver

Check [GPIO folder](GPIO)

# I2C driver

Build a I2C driver to control PCF8574 module, turn on a specific GPIO when ``insmod`` successfully: [i2c_driver_pcf8574.c](i2c_driver_pcf8574.c)

After ``insmod`` the driver, device ``/dev/PCF8574_DRIVER`` will be available for userspace communication.

## Control GPIO of PCF8574 module through the device from user space

In driver code, change ``dev_write()`` function to:

```c
#define WRITE_SIZE	1
char write_value[WRITE_SIZE];
ssize_t dev_write(struct file*filep, const char __user *buf, size_t len, loff_t *offset)
{
	memset(write_value, 0, sizeof(write_value));
	copy_from_user(write_value, buf, len);

	//Then turn on a specifc GPIO when insmod successfully
	i2c_master_send(pcf8574_i2c_client, write_value, WRITE_SIZE);
	return sizeof(write_value);
}
```

Userspace program to control GPIO by ``write()`` system call:

```c
#define DEV_NAME    "/dev/PCF8574_DRIVER"
#define WRITE_SIZE	1
#define PIN         4

char write_string[WRITE_SIZE];

int fd;

int main(){
    fd = open(DEV_NAME, O_RDWR);
    if(fd < 0) {
        printf("Fail to open %s\n",DEV_NAME);
        return 1;
    }
    else {
        write_string[0] = 1 << PIN;
        write(fd, write_string, sizeof(write_string));
    }
    return 0;
}
```

# Device tree

Check [device tree document folder](Device%20tree)

# Platform driver

Use platform driver to read properties of an overlay node: 

* Feature description:
* Source code: [read_device_tree_node_properties.c](https://github.com/TranPhucVinh/Raspberry-Pi-C/blob/main/Kernel/Device%20tree/read_device_tree_node_properties.c)

Interrupt implementation for platform driver and device tree: [Interrupt](https://github.com/TranPhucVinh/Raspberry-Pi-C/blob/main/Kernel/Device%20tree/Interrupt.md)