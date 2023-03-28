**Precondition**: Ubuntu 20.04 which has GCC 9.4.0 by default

# Overview

The general idea is to build a docker which has the Raspberry Pi board's environment, which wished to add/modify the device tree node.

# Step 1: Clone repositories

Clone https://github.com/raspberrypi/tools into ``~/tool``

```sh
git clone https://github.com/raspberrypi/tools ~/tools
```

On the Raspberry Pi board that wished to add/modify the device tree node, find which Linux version that board is using by ``uname -r`` command.

If the Linux version is 5.15, the next step is to clone the ``rpi-5.15.y`` branch of [raspberrypi/linux](https://github.com/raspberrypi/linux)

```sh
git clone --depth=1 -b rpi-5.15.y https://github.com/raspberrypi/linux
```

Folder ``linux`` will then be available

# Step 2: Create/modify node information in dtsi file

Inside the newly cloned ``linux`` folder, get to ``arch/arm/boot/dts/`` then create a ``dtsi`` file with the name ``my_dtsi.dtsi`` to add a new node:

```sh
/ {
    compatible = "brcm,bcm2835";

    new_dt_node {
        compatible = "some_string";
    };

};
```

As the board is **Raspberry Pi 3 Model B Rev 1.2**, choose file ``bcm2710-rpi-3-b.dts`` and include ``my_dtsi.dtsi`` inside it:

```c
/dts-v1/;

#include "bcm2710.dtsi"
#include "bcm2709-rpi.dtsi"
#include "bcm283x-rpi-lan7515.dtsi"
#include "bcm283x-rpi-csi1-2lane.dtsi"
#include "bcm283x-rpi-i2c0mux_0_44.dtsi"
#include "bcm271x-rpi-bt.dtsi"
#include "my_dtsi.dtsi"
```

# Step 3: Setup toolchain

```shell
export PATH=<your relative path>/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin:$PATH
export TOOLCHAIN=<your relative path>/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
```

```shell
export PATH=/home/tranphucvinh/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin:$PATH
export TOOLCHAIN=/home/tranphucvinh/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
```

# Step 4: Build and insert the dtb file

Inside the newly cloned ``linux`` folder, set default config

```sh
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
```

Run this target will create ``.config``

**Note**: If running this on Ubuntu 16.04, which has GCC 5.0 by default, there is an error

```
***
*** Compiler is too old.
***   Your GCC version:    4.8.3
***   Minimum GCC version: 5.1.0
***
scripts/Kconfig.include:44: Sorry, this compiler is not supported.
scripts/kconfig/Makefile:94: recipe for target 'bcm2709_defconfig' failed
make[1]: *** [bcm2709_defconfig] Error 1
Makefile:620: recipe for target 'bcm2709_defconfig' failed
make: *** [bcm2709_defconfig] Error 2
```

That's why we need to perform all those steps in Ubuntu 20.04

Next step is to build the device tree source code by running

```shell
make dtbs
```

After finishing building the ``bcm2710-rpi-3-b.dtb``, we copy it into the ``/boot`` directory of the Rasberry Pi, i.e replacing the original one. Then reboot the system.

After unpack the device tree, we get the result as expected

```sh
/dts-v1/;

/ {
	compatible = "raspberrypi,3-model-b-plus\0brcm,bcm2837";
	serial-number = "000000001083523b";
	model = "Raspberry Pi 3 Model B Rev 1.2";
	memreserve = <0x3b400000 0x4c00000>;
	interrupt-parent = <0x01>;
	#address-cells = <0x01>;
	#size-cells = <0x01>;

	new_dt_node {
		compatible = "some_string";
	};

	reserved-memory {
		ranges;
		#address-cells = <0x01>;
		#size-cells = <0x01>;
		phandle = <0x37>;

		linux,cma {
			reusable;
			compatible = "shared-dma-pool";
			size = <0x10000000>;
			phandle = <0x38>;
			linux,cma-default;
		};
	};
        ...
```
