For a standard Raspbian booted from SD card, 3 MMC device files will be available: **/dev/mmcblk0**, **/dev/mmcblk0p1** and **/dev/mmcblk0p2**

To use **mmc** commands, install: **sudo apt install mmc-utils**.

# /boot

``boot`` folder (``/boot``) contains all files related to the booting process, include:

* ``dtb``
* Kernel image (``img``)