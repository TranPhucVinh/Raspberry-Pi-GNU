# linux/gpio.h

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