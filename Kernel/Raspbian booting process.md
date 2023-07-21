The Raspbian booting process includes:
* **First stage bootloader**: This is used to mount the FAT32 boot partition on the SD card so that the second stage bootloader can be accessed. It is programmed into the SoC itself during manufacture of the RPi and cannot be reprogrammed by a user.
* **Second stage bootloader** (**bootcode.bin**): This is used to retrieve the GPU firmware from the SD card, program the firmware, then start the GPU.
* **GPU firmware** (**start.elf**): Once loaded, this allows the GPU to start up the CPU. An additional file, fixup.dat, is used to configure the SDRAM partition between the GPU and the CPU. At this point, the CPU is release from reset and execution is transferred over.
* **User code**: This can be one of any number of binaries. By default, it is the Linux kernel (usually named **kernel.img**), but it can also be another bootloader (e.g. **U-Boot**), or a bare-bones application.
