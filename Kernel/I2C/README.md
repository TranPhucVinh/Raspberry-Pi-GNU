# API
## i2c_add_adapter()
```c
int i2c_add_adapter(struct i2c_adapter *adapter);
```

Declare the I2C adapter by using a dynamic bus ID.

## struct i2c_adapter
```c
struct i2c_adapter {
	struct module *owner;
	unsigned int class;		  /* classes to allow probing for */
	const struct i2c_algorithm *algo; /* the algorithm to access the bus */
	void *algo_data;

	/* data fields that are valid for all devices	*/
	const struct i2c_lock_operations *lock_ops;
	struct rt_mutex bus_lock;
	struct rt_mutex mux_lock;

	int timeout;			/* in jiffies */
	int retries;
	struct device dev;		/* the adapter device */

	int nr;
	char name[48];
	struct completion dev_released;

	struct mutex userspace_clients_lock;
	struct list_head userspace_clients;
};
```
# I2C driver to control PCF8574 module
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
