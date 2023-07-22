The Raspbian booting process includes:
* **Step 1: First stage bootloader**: The on-chip boot ROM is started. This is used to mount the FAT32 boot partition on the SD card so that the second stage bootloader can be accessed. It is programmed into the SoC itself during manufacture of the RPi and cannot be reprogrammed by a user.
* **Step 2: Second stage bootloader** (**bootcode.bin**): This is used to retrieve the GPU firmware from the SD card, program the firmware, then start the GPU. By that, **bootcode.bin** is also called the GPU bootloader.
* **Step 3**: **start.elf as GPU firmware**: **start.elf** is loaded. Once loaded, **start.elf** allows the GPU to start up the CPU. An additional file, **fixup.dat**, is used to configure the SDRAM partition between the GPU and the CPU. At this point, the CPU is release from reset and execution is transferred over.
* **Step 4**: **start.elf** loads **kernel.img**. It then also reads **config.txt**, **cmdline.txt** and **device tree binary files**.
* **Step 5**: ``kernel.img`` is then run on the ARM.

**Note**: It's very important to note that on Raspbian, everything runs on the GPU until ``kernel.img`` is loaded on the ARM.
