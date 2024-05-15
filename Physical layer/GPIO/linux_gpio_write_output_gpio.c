#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

#define GPIOCHIP_DEV        "/dev/gpiochip0"
#define LED                 15
#define REQ_ONE_GPIO        1 // Request a single GPIO

int main() {
    int fd;

    // Open GPIO chip device for writing
    fd = open(GPIOCHIP_DEV, O_RDWR);
    if (fd < 0) {
        perror("Error opening GPIO chip device for writing");
        return -1;
    }

    struct gpiohandle_request req;
 
    // Set GPIO pin direction to output
    req.lineoffsets[0] = LED;
    req.flags = GPIOHANDLE_REQUEST_OUTPUT;
    req.lines = REQ_ONE_GPIO;
    req.default_values[0] = 0;

    if (ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0) {
        perror("Error setting GPIO pin direction to output");
        close(fd);
        return -1;
    }

    struct gpiohandle_data data;

    data.values[0] = 1; // GPIO_PIN HIGH
    if (ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        perror("Error setting GPIO pin value");
        close(req.fd);
        close(fd);
        return -1;
    }

    sleep(5);

    data.values[0] = 0; // GPIO_PIN LOW
    if (ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        perror("Error setting GPIO pin value");
        close(req.fd);
        close(fd);
        return -1;
    }

    close(req.fd);// Close GPIO chip device
    close(fd);
}