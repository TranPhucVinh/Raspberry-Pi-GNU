#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <string.h>

#define GPIOCHIP_DEV "/dev/gpiochip0"
#define GPIO_PIN    15
#define TIMEOUT     5000    // milliseconds
#define MAXEVENTS   1       // Monitor 1 file descriptor/1 terminal

struct epoll_event monitored_event, happened_event;

int main() {
    int gpiochip_fd, epoll_fd, nfds;
    struct gpioevent_request req;
    struct gpioevent_data event;

    // Open GPIO chip device
    gpiochip_fd = open(GPIOCHIP_DEV, O_RDWR | O_NONBLOCK);
    if (gpiochip_fd < 0) {
        perror("Error opening GPIO chip device");
        return -1;
    }

    // Request GPIO event
    memset(&req, 0, sizeof(req));
    req.lineoffset = GPIO_PIN;
    req.handleflags = GPIOHANDLE_REQUEST_INPUT;
    req.eventflags = GPIOEVENT_REQUEST_RISING_EDGE;

    if (ioctl(gpiochip_fd, GPIO_GET_LINEEVENT_IOCTL, &req) < 0) {
        perror("Error requesting GPIO event");
        close(gpiochip_fd);
        return -1;
    }

    // Set up epoll instance
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) {
        perror("Error creating epoll instance");
        close(req.fd);
        close(gpiochip_fd);
        return -1;
    }

    // Add GPIO file descriptor to epoll set
    monitored_event.events = EPOLLIN | EPOLLET; // Edge-triggered mode
    monitored_event.data.fd = req.fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, req.fd, &monitored_event) < 0) {
        perror("Error adding GPIO file descriptor to epoll set");
        close(req.fd);
        close(gpiochip_fd);
        close(epoll_fd);
        return -1;
    }

    int total_irq = 0;
    
    while (1) {
        nfds = epoll_wait(epoll_fd, &happened_event, MAXEVENTS, TIMEOUT);
        if (nfds < 0) {
            perror("Error in epoll_wait");
            close(req.fd);
            close(gpiochip_fd);
            close(epoll_fd);
            return -1;
        }

        // Handle edge-triggered interrupt event
        if (happened_event.events & EPOLLIN) {
            // As using GPIO_GET_LINEEVENT_IOCTL, must read GPIO event data to clear interrupt
            if (read(req.fd, &event, sizeof(event)) != sizeof(event)) {
                perror("Error reading GPIO event data");
                close(req.fd);
                close(gpiochip_fd);
                close(epoll_fd);
                return -1;
            }

            total_irq += 1;

            if (event.id == GPIOEVENT_EVENT_RISING_EDGE) {
                printf("GPIOEVENT_EVENT_RISING_EDGE occurred on GPIO %d for %d times\n", GPIO_PIN, total_irq);
            }
        }
    }

    close(req.fd);
    close(gpiochip_fd);
    close(epoll_fd);

    return 0;
}
