#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define PATH "/sys/class/gpio/"
#define LED	"14"

char path[50];
int fd;
int main()
{  
	sprintf(path, "%sexport", PATH);

	//The mode must be O_WRONLY. Mode O_RDWR result in fd=-1
	fd = open(path, O_WRONLY);
	if(fd < 0) return 1;

    write(fd, LED, sizeof(LED));
	close(fd);
	
	sprintf(path, "%sgpio%s/direction", PATH, LED);
	fd = open(path, O_RDWR);//defined in fcntl.h
	if(fd < 0) return 1;

	write(fd, "OUT", sizeof("OUT"));
	close(fd);

	sprintf(path, "%sgpio%s/value", PATH, LED);
	fd = open(path, O_RDWR);//defined in fcntl.h
	if(fd < 0) return 1;

    while(1){
        write(fd, "1", sizeof("1"));
        sleep(1);
        write(fd, "0", sizeof("0"));
        sleep(1);
    }
}