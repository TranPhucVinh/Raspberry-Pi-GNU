# Compiler

WiringPi library: ``gcc main.c -lwiringPi``

## arm-linux-gnueabihf-gcc

Compiler ``arm-linux-gnueabihf-gcc`` is available on Raspbian: ``arm-linux-gnueabihf-gcc test.c``. ``a.out`` file will then be the output.

For Ubuntu, install ``sudo apt install gcc-arm-linux-gnueabihf``, then ``arm-linux-gnueabihf-gcc`` is available.

Other flag works like ``GCC``, like [-I flag](https://github.com/TranPhucVinh/C/blob/master/Environment/GCC%20compiler.md#include-directory-of-header-files-with--i): ``arm-linux-gnueabihf-gcc -Imain main.c``

For CPP source code with CPP lib like ``iostream``, use linker ``stdc++`` like when compiling with GCC: ``arm-linux-gnueabihf-gcc test.cpp -lstdc++``

# Github project environment

[Images used in that projects](Images)
