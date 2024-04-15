GPIO with sysfs can be configured to handle interrupt. 

In order to enable interrupt, ``/sys/class/gpio/<gpio_pin_num>/edge`` must be set with "rising" or "falling" for the corresponding edge-triggered IRQ.

After handling the interrupt, lseek() to the beginning of the sysfs file then read GPIO value to **clear the interrupt**. When you read from the GPIO value file ``/sys/class/gpio/<gpio_pin_num>/value``, the kernel typically updates the GPIO pin's interrupt status to indicate that the interrupt has been acknowledged and handled.

# Detect the interrupt connected to a GPIO for every time there is an edge-triggered
Connected a button to a GPIO. Everytime pressing that button, interrupt with edge-triggered will occur:
```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

#define GPIO_PIN        15
#define MAX_EVENTS      2
#define EPOLL_TIMEOUT   1000 // milliseconds

int main() {
    int gpio_fd, epoll_fd;
    struct epoll_event monitored_event, happened_event;

    // Open epoll file descriptor
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) {
        perror("Error creating epoll file descriptor");
        close(gpio_fd);
        return 1;
    }

    // Open GPIO pin for reading (input)
    gpio_fd = open("/sys/class/gpio/gpio15/value", O_RDONLY);
    if (gpio_fd < 0) {
        perror("Error opening GPIO value file");
        return 1;
    }

    // Add GPIO file descriptor to epoll instance
    monitored_event.events = EPOLLIN | EPOLLET; // Edge-triggered mode
    monitored_event.data.fd = gpio_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, gpio_fd, &monitored_event) < 0) {
        perror("Error adding GPIO file descriptor to epoll");
        close(gpio_fd);
        close(epoll_fd);
        return 1;
    }

    printf("Waiting for GPIO interrupt on pin %d...\n", GPIO_PIN);

    // Main loop to wait for GPIO interrupts
    while (1) {
        int total_ready_fd = epoll_wait(epoll_fd, &happened_event, MAX_EVENTS, EPOLL_TIMEOUT);
        if (total_ready_fd < 0) {
            perror("epoll_wait error");
            return -1;
        } else if (total_ready_fd == 1){
		/*
			happened_event.events is a bitmask, not a single value. To check if the EPOLLIN 
			event has occurred, you should use the & (bitwise AND) operator
    	*/
            if (happened_event.events & EPOLLIN) {
                printf("GPIO interrupt occurred on pin %d\n", GPIO_PIN);

                // After handling the interrupt, lseek() to the beginning of the sysfs
		        // file then read GPIO value to clear interrupt
                lseek(gpio_fd, 0, SEEK_SET); // Reset file offset
                char buf;
                read(gpio_fd, &buf, sizeof(buf));
            }
        }
    }

    // Cleanup
    close(gpio_fd);
    close(epoll_fd);
    return 0;
}
```
