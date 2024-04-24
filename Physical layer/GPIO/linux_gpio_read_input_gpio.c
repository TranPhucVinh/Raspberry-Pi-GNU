#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

#define GPIOCHIP_DEV        "/dev/gpiochip0"
#define GPIO_PIN            56
#define REQ_ONE_GPIO        1 // Request a single GPIO

int main() {
    int fd;
    struct gpiohandle_request req;

    // Open GPIO chip device for writing
    fd = open(GPIOCHIP_DEV, O_RDWR);
    if (fd < 0) {
        perror("Error opening GPIO chip device for writing");
        return -1;
    }

    // Set GPIO pin direction to input
    req.lineoffsets[0] = GPIO_PIN;
    req.flags = GPIOHANDLE_REQUEST_INPUT;
    req.lines = REQ_ONE_GPIO;

    if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0) {
        perror("Error setting GPIO pin direction to output");
        close(fd);
        return -1;
    }

    struct gpiohandle_data data;

    // Read GPIO 56 value
    if (ioctl(req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
        perror("Error getting GPIO pin value");
        close(req.fd);
        close(fd);
        return -1;
    }

    printf("GPIO value %d\n",  data.values[0]);

    close(fd);
}
