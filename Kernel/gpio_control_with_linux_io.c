#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h> //For msleep()
#include <linux/io.h>
#include <asm/io.h>

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

#define LED			4

#define DELAY       1000

#define MAPPING_SIZE    0x100 //MAPPING_SIZE should be a valid value and big enough to avoid memory crashing in ioremap()

MODULE_LICENSE("GPL");

struct 	task_struct *kthread_1;

unsigned int *gpio_base;

int thread_function(void *kernel_data) {
	while(!kthread_should_stop()){
        GPIO_SET(gpio_base, LED);
        msleep(DELAY);
        GPIO_CLR(gpio_base, LED);
        msleep(DELAY);
	}
	return 0;
}

int init_module(void)
{
	printk(KERN_INFO "Hello, World !\n");

    //Memory mapping
    gpio_base = (unsigned int *)ioremap(GPIO_BASE, MAPPING_SIZE);
    if (gpio_base == NULL) printk("Couldn't perform mapping\n");
    else {
		//Thread setup
		kthread_1 = kthread_create(thread_function, NULL, "kthread_1");
		if(kthread_1 != NULL){
			wake_up_process(kthread_1); //Start kthread_1
			printk("kthread_1 was created and is running now!\n");
		}
		else {
			printk("kthread_1 could not be created!\n");
			return -1;
		}
		GPIO_OUT(gpio_base, LED);
	}

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
	iounmap(gpio_base);
	kthread_stop(kthread_1);
}
