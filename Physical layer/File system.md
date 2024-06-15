For a standard Raspbian booted from SD card, 3 MMC device files will be available: **/dev/mmcblk0**, **/dev/mmcblk0p1** and **/dev/mmcblk0p2**

To use **mmc** commands, install: **sudo apt install mmc-utils**.

# /boot

``boot`` folder (``/boot``) contains all files related to the booting process, include:

* ``dtb``
* Kernel image (``img``)

# Script running when system booted

## crontab

**Example**: Setup file ``a.out`` which [blink LED every 1 second by direct memory access](https://github.com/TranPhucVinh/Raspberry-Pi-C/blob/main/Physical%20layer/GPIO/direct_register_access_control_gpio.c) to run every 1 minute

Edit crontab file by adding this line

```sh
* * * * * /home/pi/work-in-progress/C_GCC_workspace/a.out
```
## Systemd

Raspbian supports systemd, which locates in ``/etc/systemd/system``, like Ubuntu and Armbian.

**Start a script/process to control GPIO**: Not yet implemented, check [the corresponding document in Orange Pi](https://github.com/TranPhucVinh/Orange-Pi/blob/master/Physical%20layer/Systemd.md#example-start-a-scriptprocess-to-blink-on-board-red-led).

## /etc/rc.local

0rigin file ``/etc/rc.local`` in Raspbian 

```sh
#!/bin/sh -e
#
# rc.local
#
# This script is executed at the end of each multiuser runlevel.
# Make sure that the script will "exit 0" on success or any other
# value on error.
#
# In order to enable or disable this script just change the execution
# bits.
#
# By default this script does nothing.

# Print the IP address
_IP=$(hostname -I) || true
if [ "$_IP" ]; then
  printf "My IP address is %s\n" "$_IP"
fi

exit 0
```
