# [Install and config]()

Install and config document includes:

* [Manually download Raspbian OS]()
* [Using Raspberry Pi Imager]()
* [First time boot](): [Connect to WiFi in first time boot](), [connect with USB in first time boot](), connect LAN cable to router, connect with USB then setup by Putty
* [VNC]()

## Keyboard setup

By default, when running the for the first time, the keyboard type is configured to be UK type so that ``|``, ``#``,... can't be typed. User need to configure the keyboard to English (US) type.

**Step 1**: ``sudo dpkg-reconfigure keyboard-configuration``. Tab ``Configuring keyboard-configuration`` will appear.

**Step 2**: In ``Keyboard model`` tab, let all options default

**Step 3**: In ``Keyboard layout`` tab, choose ``Other``, then choose ``English (UK)``.

**Step 4**: In ``Key to function as AltGr`` tab, let all options default

**Step 5**: In ``Compose key`` tab, let all options default

After successfully setting up, reboot the board to take effect.

# Binary file

Error ``cannot execute binary file: Exec format error``

This error happens as running an executable file compiled for an ARM architecture on an x86-64 architecture. Like running file ``a.out`` compiled in Ubuntu 16.04 on Raspberry Pi 3B+.

# Compiler

``WiringPi`` is a C/C++ library for Raspberry Pi.

Compile: ``gcc main.c -lwiringPi``

## arm-linux-gnueabihf-gcc

Compiler ``arm-linux-gnueabihf-gcc`` is available on Raspbian: ``arm-linux-gnueabihf-gcc test.c``. ``a.out`` file will then be the output.

For Ubuntu, install ``sudo apt install gcc-arm-linux-gnueabihf``, then ``arm-linux-gnueabihf-gcc`` is available.

Other flag works like ``GCC``, like [-I flag](https://github.com/TranPhucVinh/C/blob/master/Environment/GCC%20compiler.md#include-directory-of-header-files-with--i): ``arm-linux-gnueabihf-gcc -Imain main.c``

For CPP source code with CPP lib like ``iostream``, use linker ``stdc++`` like when compiling with GCC: ``arm-linux-gnueabihf-gcc test.cpp -lstdc++``

## arm-linux-gnueabi-g++

Install: ``sudo apt install g++-arm-linux-gnueabi``

Compile: ``arm-linux-gnueabi-g++ main.cpp``

# Github project environment

[Images used in that projects](Images)
