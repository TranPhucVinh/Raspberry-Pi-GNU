#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define GPIO_PATH   "/sys/class/gpio/gpio"

#define BUTTON	    "15"

char    button_path[50];
int     button_fd;

int main()
{  
    sprintf(button_path, "%s%s/direction", GPIO_PATH, BUTTON);
	button_fd = open(button_path, O_RDWR);
	if(button_fd < 0) {
        printf("Unable to open %s%s/direction\n", GPIO_PATH, BUTTON);
        return -1;
    }

    write(button_fd, "in", sizeof("in"));
	close(button_fd);

    sprintf(button_path, "%s%s/value", GPIO_PATH, BUTTON);
   
    // Must open file /sys/class/gpio/gpio15/value to read it repeatedly
    // If only open it for 1 time to read, just only the 1st written value is read
    while(1){
        button_fd = open(button_path, O_RDONLY);
        if(button_fd < 0) {
            printf("Fail to open %s\n", button_path);
            return -1;
        }

        char btn_val[2];
        read(button_fd, btn_val, sizeof(btn_val));
        printf("Button value %s\n", btn_val);

        close(button_fd);
        usleep(10000); // 10ms
    }
}