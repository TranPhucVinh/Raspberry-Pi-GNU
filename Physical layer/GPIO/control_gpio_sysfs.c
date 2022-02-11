#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define PATH "/sys/class/gpio/"
#define LED	14

char path[50];
int fd;
int main()
{  
	sprintf(path, "%sexport", PATH);

	//The mode must be O_WRONLY. Mode O_RDWR result in fd=-1
	fd = open(path, O_WRONLY);
	if(fd < 0) {
		printf("Fail %s", path);
		return 1;
	}
	else write(fd, "14", 3);
	close(fd);
	
	sprintf(path, "%sgpio%d/direction", PATH, LED);
	fd = open(path, O_RDWR);//defined in fcntl.h
	if(fd < 0) 
	{
		printf("Fail %s", path);
		return 1;
	}
	else write(fd, "OUT", 4);
	close(fd);

	sprintf(path, "%sgpio%d/value", PATH, LED);
	fd = open(path, O_RDWR);//defined in fcntl.h
	if(fd < 0) 
	{
		printf("Fail %s", path);
		return 1;
	}
	else {
		while(1){
			write(fd, "1", 2);
			sleep(1);
			write(fd, "0", 2);
			sleep(1);
		}
	}
}