# Fundamental concepts

A devicetree (also written device tree, full name **Open Firmware Device Tree**, abbreviated **dt**) is a data structure describing the hardware components of a particular computer so that the operating system's kernel can use and manage those components, including the CPU or CPUs, the memory, the buses and the peripherals. With device tree supported, the operating system doesn't need to hard code its machine details. Structurally, the device tree is a tree with named nodes, and nodes may have an arbitrary number of named properties encapsulating arbitrary data.

In Raspbian or Unix single board computers in general, device tree are opened to be read from **userspace** in ``/sys/firmware/devicetree/base``

## dtc command

dtc command stands for **Device tree compiler**

Install (especially on Other Linux Distro like Ubuntu): ``sudo apt install device-tree-compiler``

To unpack the device tree and view on terminal: ``dtc -I fs /sys/firmware/devicetree/base``

Unpack the device tree to a file: ``(dtc -I fs /sys/firmware/devicetree/base) > device_tree.md``

## Device tree files

* ``.dts``: device tree source files for board-level definitions
* ``.dtsi``: device tree source include files
* ``.dtb``: Blob file as the device tree source is compiled into the binary format. The format of the data in the ``.dtb`` blob file is commonly referred to as a **Flattened Device Tree (FDT)**. The FDT is accessed in the raw form during the very early phases of boot, but is expanded into a kernel internal data structure known as the **Expanded Device Tree (EDT)** for more efficient access for later phases of the boot and after the system has completed booting.
## [Device tree overlay](Device%20tree%20overlay.md)

A device tree overlay purpose is to modify the kernel’s live tree. Device tree overlay implementation includes:
* [Create an overlay device tree node](Device%20tree%20overlay.md#create-an-overlay-device-tree-node): [Add an overlay node with label](Device%20tree%20overlay.md#add-an-overlay-node-with-label), [overlay device tree add value to other target](Device%20tree%20overlay.md#overlay-device-tree-add-value-to-other-target) and [add multiple fragments in overlay device tree](Device%20tree%20overlay.md#add-multiple-fragments-in-overlay-device-tree)
* [Read all device tree nodes, included device tree overlay nodes](Device%20tree%20overlay.md#remove-the-inserted-device-tree-overlay-nodes)
* [Remove the inserted device tree overlay nodes](Device%20tree%20overlay.md#remove-the-inserted-device-tree-overlay-nodes)
* [Using device tree overlay for GPIO control by kernel API](Device%20tree%20GPIO.md)

# Data type

## Array

**Array** in device tree are inside ``<>``, each member is seperated by space. ``,`` is not allowed to used to seperate each member.

E.g: An int array properties inside a devie tree node:

```c
int_array	 = <1 2 3 4>;
```
All properties with a single int value, e.g ``phandle = <0x2>`` are always array so they must be read by [of_property_read_u32_array()](API.md#of_property_read_u32_array) function, using [of_find_property()](API.md#of_find_property) to read them result in garbage value.

## [Interrupt](Interrupt.md)

* [Device tree interrupt properties](Interrupt.md#properties): ``interrupts``, ``interrupt-parent``, ``interrupt-cells`` and ``interrupt-controller``
* [Interrupt implementation for platform driver](Interrupt.md#interrupt-implementation-for-platform-driver): [GPIO interrupt](Interrupt.md#gpio-interrupt), [get interrupt number by name](Interrupt.md#get-interrupt-number-by-name)

## device_type

Use [of_find_node_by_type()](API.md#of_find_node_by_type) to find a node by its ``device_type`` property.

# [Operations for device tree node properties](Operations%20for%20device%20tree%20node%20properties.md)

[Operations for device tree node properties](Operations%20for%20device%20tree%20node%20properties.md) includes:
* [Read device tree node properties](Operations%20for%20device%20tree%20node%20properties.md#read-device-tree-node-properties) by [using platform driver function](Operations%20for%20device%20tree%20node%20properties.md#using-platform-driver-function) and [using overlay device tree function](Operations%20for%20device%20tree%20node%20properties.md#using-overlay-device-tree-function)
* [Create operation for device tree](Operations%20for%20device%20tree%20node%20properties.md#create-operation-for-device-tree): Add a new property to an overlay node
* [Update device tree node properties](Update%20device%20tree%20node%20properties.md) with various types: string, int and array; [update refcount of a device tree node](Update%20device%20tree%20node%20properties.md#update-refcount-of-a-device-tree-node) by [of_node_get()](API.md#of_node_get) and [of_node_put()](API.md#of_node_put)
* [Delete operation for device tree](Operations%20for%20device%20tree%20node%20properties.md#delete-operation-for-device-tree): Remove property and remove/detach a node
* [Add a new node to device tree by dtsi file modification]()
* [Remove a property of a node and add a new empty property (i.e property without any value)](Operations%20for%20device%20tree%20node%20properties.md#remove-a-property-of-a-node-and-add-a-new-empty-property-ie-property-without-any-value)
# [API](API.md)

* [All device tree struct](API.md#struct)
* [of_find_compatible_node()](API.md#of_find_compatible_node)
* [of_find_node_by_type()](API.md#of_find_node_by_type)
* [of_find_property()](API.md#of_find_property)
* [of_property_read_u32_array()](API.md#of_property_read_u32_array)
* [of_node_get()](API.md#of_node_get) and [of_node_put()](API.md#of_node_put)
* [kbasename()](API.md#kbasename)
* [for_each_child_of_node()](API.md#for_each_child_of_node)
