As Raspberry Pi 3B+ board doesn't have RTC clocks by default, ``sudo hwclock -r`` will fail:

```
hwclock: Cannot access the Hardware Clock via any known method.
hwclock: Use the --verbose option to see the details of our search for an access method.
```

``sudo hwclock -r -v``: Show current date in Unix timestamps (in seconds) along with log like this

```
hwclock from util-linux 2.36.1
System Time: 1670076207.544261
Trying to open: /dev/rtc0
Trying to open: /dev/rtc
Trying to open: /dev/misc/rtc
No usable clock interface found.
hwclock: Cannot access the Hardware Clock via any known method.
```

Those log are printed out as device files ``/dev/rtc``, ``/dev/rtc0`` and ``/dev/misc/rtc`` are not existed in Raspberry Pi 3B+ board. 

That's why we need the external RTC module. To read time with ``hwclock`` command by DS1307 module, check [DS1307 module document](I2C.md#DS1307).