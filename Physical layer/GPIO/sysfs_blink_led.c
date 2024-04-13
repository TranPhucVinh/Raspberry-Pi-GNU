#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define PATH "/sys/class/gpio/gpio"
#define LED	"14"

char led_path[50];
int fd;
int main()
{  
	sprintf(led_path, "%s%s/direction", PATH, LED);

	fd = open(led_path, O_RDWR);
	if(fd < 0) {
        printf("Unable to open %s%s/direction\n", PATH, LED);
        return -1;
    }

	if (!write(fd, "out", sizeof("out"))) printf("Unable to write output to GPIO %s\n", LED);
    
	close(fd);

	sprintf(led_path, "%s%s/value", PATH, LED);
	fd = open(led_path, O_WRONLY);
	if(fd < 0) return 1;

    while(1){
        write(fd, "1", sizeof("1"));
        sleep(1);
        write(fd, "0", sizeof("0"));
        sleep(1);
    }
}