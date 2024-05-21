# linux/gpio.h
## struct gpiochip_info
```c
/**
 * struct gpiochip_info - Information about a certain GPIO chip
 * @name: the Linux kernel name of this GPIO chip
 * @label: a functional name for this GPIO chip, such as a product
 * number, may be NULL
 * @lines: number of GPIO lines on this chip
 */
struct gpiochip_info {
	char name[32];
	char label[32];
	__u32 lines;
};
```
## struct gpiohandle_request
```c
struct gpiohandle_request {
	__u32 lineoffsets[GPIOHANDLES_MAX];
	__u32 flags;
	__u8 default_values[GPIOHANDLES_MAX];
	char consumer_label[32];
	__u32 lines;
	int fd;
};
```
* **lineoffsets**: an array desired lines, specified by offset index for the associated GPIO device
* **lines**: number of lines requested in this request, i.e. the number of valid fields in the above arrays. **Set to 1 to request a single line**.
* **fd**: if successful, this field will contain a valid anonymous file handle after a **GPIO_GET_LINEHANDLE_IOCTL** operation, zero or negative value means error

## struct gpiohandle_data
Information of values on a GPIO handle
```c
struct gpiohandle_data {
	__u8 values[GPIOHANDLES_MAX];
};
```
* **values**: When getting the state of lines, this contains the current state of a line. When setting the state of lines, these should contain the desired target state

## struct gpioevent_data
```c
/**
 * struct gpioevent_data - The actual event being pushed to userspace
 * @timestamp: best estimate of time of event occurrence, in nanoseconds
 * @id: event identifier
 */
struct gpioevent_data {
	__u64 timestamp;
	__u32 id;
};
```
``id`` event identifier includes:
```c
#define GPIOEVENT_EVENT_RISING_EDGE 1
#define GPIOEVENT_EVENT_FALLING_EDGE 2
```
# Get chip_info by ioctl() GPIO_GET_CHIPINFO_IOCTL

```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

#define GPIOCHIP_DEV "/dev/gpiochip0"

int main() {
    int     fd;
    struct  gpiochip_info chip_info;

    // Open GPIO chip device
    fd = open(GPIOCHIP_DEV, O_RDWR);
    if (fd < 0) {
        perror("Error opening GPIO chip device");
        return -1;
    }

    // Get GPIO chip information
    if (ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &chip_info) < 0) {
        perror("Error getting GPIO chip information");
        close(fd);
        return -1;
    }

    printf("%s %s %d\n", chip_info.name, chip_info.label, chip_info.lines);// gpiochip0 pinctrl-bcm2835 54

    close(fd);
}
```
# Read from input GPIO

**Feature**: Read value of an input GPIO

**Program**: [linux_gpio_read_input_gpio.c](linux_gpio_read_input_gpio.c)

# Write to output GPIO

**Feature**: Turn ON/OFF LED

**Program**: [linux_gpio_write_output_gpio.c](linux_gpio_write_output_gpio.c)
# Handle IRQ with epoll

In order to use epoll IRQ for Linux GPIO, ``GPIO_GET_LINEEVENT_IOCTL`` ioctl() macro must be used: [linux_gpio_irq_epoll.c](linux_gpio_irq_epoll.c)
