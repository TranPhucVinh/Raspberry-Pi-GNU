#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define GPIO_PATH        "/sys/class/gpio/gpio"

#define LED	        "14"
#define BUTTON	    "15"

char    led_path[50], button_path[50];
int     led_fd, button_fd;

int main()
{  
	sprintf(led_path, "%s%s/direction", GPIO_PATH, LED);
    sprintf(button_path, "%s%s/direction", GPIO_PATH, BUTTON);

	led_fd = open(led_path, O_RDWR);
	if(led_fd < 0) {
        printf("Unable to open %s%s/direction\n", GPIO_PATH, LED);
        return -1;
    }

    button_fd = open(button_path, O_RDWR);
	if(button_fd < 0) {
        printf("Unable to open %s%s/direction\n", GPIO_PATH, BUTTON);
        return -1;
    }

	if (!write(led_fd, "out", sizeof("out"))){
        printf("Unable to write output to GPIO %s\n", LED);
        return -1;
    } 

    close(led_fd);

    if (!write(button_fd, "in", sizeof("in"))){
        printf("Unable to write output to GPIO %s\n", LED);
        return -1;
    }

    close(button_fd);

    sprintf(button_path, "%s%s/value", GPIO_PATH, BUTTON);
	sprintf(led_path, "%s%s/value", GPIO_PATH, LED);

	led_fd = open(led_path, O_WRONLY);
	if(led_fd < 0) return 1;

    while(1){
        button_fd = open(button_path, O_RDONLY);
        if(button_fd < 0) {
            printf("Fail to open %s\n", button_path);
            return -1;
        }

        char btn_val[2];
        read(button_fd, btn_val, sizeof(btn_val));
        write(led_fd, btn_val, sizeof(btn_val));
        close(button_fd);
        
        usleep(10000); // 10ms
    }
}