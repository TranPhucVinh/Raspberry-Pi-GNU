The Raspbian booting process includes:
* **Step 1: First stage bootloader**: The on-chip boot ROM is started. This is used to mount the FAT32 boot partition on the SD card so that the second stage bootloader can be accessed. It is programmed into the SoC itself during manufacture of the RPi and cannot be reprogrammed by a user.
* **Step 2: Second stage bootloader** (**bootcode.bin**): This is used to do some basic setup, retrieve **start.elf** (the GPU firmware) from the SD card, program the firmware, then start the GPU. By that, **bootcode.bin** is also called the GPU bootloader.
* **Step 3**: **start.elf as GPU firmware**: **start.elf** is loaded. Once loaded, **start.elf** allows the GPU to start up the CPU. **fixup.dat**, a linker file, is used to configure the SDRAM partition between the GPU and the CPU. At this point, the CPU is release from reset and execution is transferred over.
* **Step 4**: **start.elf** read **config.txt** to get the booting information like which kernel image file to load,... **start.elf** will then load the kernel image, like **kernel.img**. It then also reads **cmdline.txt** and **device tree binary files**.
* **Step 5**: **kernel.img** is then run on the RAM

**Note**: It's very important to note that on Raspbian, everything runs on the GPU until **kernel.img** is loaded on the RAM.

It can also be noted that the booting process of Raspbian **doesn't require a Uboot to load kernel.img** as in **step 4**, **start.elf**, which is the GPU firmware, loads **kernel.img**.
# config.txt
The Raspberry Pi uses a configuration file instead of the BIOS in a conventional PC. The system configuration parameters, which would traditionally be edited and stored using a BIOS, are stored instead in an optional text file named **config.txt**. This is read by the GPU firmware, **start.elf**, before the ARM CPU and Linux are initialised. It must therefore be located on the first (boot) partition of your SD card, alongside **bootcode.bin** and **start.elf**.

**config.txt file format**:
* ``property=value`` to setup ``value`` for ``property``
* ``#`` for comment

**Property value**:

* ``enable_uart``: ``enable_uart=1`` requests that the kernel creates a serial console, accessible using GPIOs 14 and 15 (pins 8 and 10 on the 40-pin header)
* ``kernel`` will specify the boot partition to use when loading the kernel. The default value on the Raspberry Pi 3 and 3+ is ``kernel7.img``, while other Raspberry Pi board takes different kernel image like kernel.img, kernel8.img,...
