### Print out string

Print out a string after every 1 second. 

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main () {
     while(1) {
		//fprintf(stdout, "Hello, World!");
		printf("Hello, World!");
		fflush(stdout);
		sleep(1);
	}
    return 0;
}

```

**Notice**: 

* ``fflush(stdout)`` will empty the buffer of stdout for proper printing out every 1 second.
* For proper ``printf()``, ``fflush(stdout)`` must be followed after.

Without ``fflush(stdout)``, the program will be:

```c
while(1) {
	printf("Hello, World!");
	sleep(1);
}
```

This program will print out a bunch of strings at one time after a few seconds.
