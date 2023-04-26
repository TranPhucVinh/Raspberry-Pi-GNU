obj-m += main.o

all:
	make -C /raspberry/linux M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
clean:
	make -C /raspberry/linux M=$(PWD) clean