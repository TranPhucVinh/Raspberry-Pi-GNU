#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>

#define BCM2837_PHY_ADDR                0x3f000000 //BCM2837 physical address start
#define GPIO_BASE                       (BCM2837_PHY_ADDR + 0x200000) /* GPIO controller */

#define GPIO_INP(addr,pin) *(addr+((pin)/10)) &= ~(7<<(((pin)%10)*3))
#define GPIO_OUT(addr,pin) {\
        *(addr+((pin)/10)) &= ~(7<<(((pin)%10)*3));\
        *(addr+((pin)/10)) |= (1<<(((pin)%10)*3));\
}
#define GPIO_SET(addr,pin) *(addr+7)=1<<pin //Set GPIO in GPSET0
#define GPIO_CLR(addr,pin) *(addr+10)=1<<pin //Clear GPIO in GPCLR0
#define GPIO_GET(addr,pin) ((*(addr+13))>>pin)&1 //GET GPIO value in GPLEV0

// #define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define short_wait for(int ii=0;ii<=200;ii++)
#define NO_PULL 0
#define PULL_D 1
#define PULL_U 2
#define GPIO_PULL(addr,state,list_of_pin) \
{\
        int pin_array[] = list_of_pin;\
        int nums = sizeof(pin_array)/sizeof(int);\
        uint32_t pulld_reg = 0;\
        for(int count = 0; count < nums; count++){\
                pulld_reg |= 1 << pin_array[count];\
        }\
        *(addr+37) = state;\
        short_wait;\
        *(addr+38) = pulld_reg;\
        short_wait;\
        *(addr+37)=0;\
        *(addr+38)=0;\
}

#define LED 4

uintptr_t setup_io();

uint32_t *gpio;

int main(int argc, char **argv)
{
        
        gpio = (uint32_t*)setup_io();
        if(gpio == 0){
                exit(1);
        }
		
        // GPIO_PULL(gpio, NO_PULL, {17});
        GPIO_OUT(gpio, LED);

        while(1){
                GPIO_SET(gpio, LED);
                sleep(1);
                GPIO_CLR(gpio, LED);
                sleep(1);
        }
        return 0;
}

uintptr_t setup_io()
{
        int fd ;
        uint32_t* mapping_address;
        /*
                open /dev/mem as a bridge for MMU mapping
                from the physical addresses to virtual address
        */
        if ((fd = open ("/dev/mem", O_RDWR | O_SYNC) ) < 0) {
                printf("Unable to open /dev/mem: \n");
                return 0;
        }

        printf("open /dev/mem ok \n");

        //mapping  GPIO base physical address to a virtual address
        mapping_address = (uint32_t *)mmap(NULL, 4048, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

        if ((intptr_t)mapping_address < 0){
                printf("mmap failed: \n");
                return 0;
        }
        printf("mmap ok \n");
        close(fd);

        return (uintptr_t)mapping_address;
}