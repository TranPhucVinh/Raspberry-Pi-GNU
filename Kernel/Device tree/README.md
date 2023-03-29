# Fundamental concepts

In Raspbian or Unix single board computers in general, device tree are opened to be read from userspace in ``/sys/firmware/devicetree/base``

To unpack the device tree and view on terminal: ``dtc -I fs /sys/firmware/devicetree/base``

Unpack the device tree to a file: ``(dtc -I fs /sys/firmware/devicetree/base) > device_tree.md``

## Device tree files

* ``.dts``: device tree source files for board-level definitions

## [Device tree overlay](Device%20tree%20overlay.md)

A device tree overlay purpose is to modify the kernel’s live tree. Device tree overlay implementation includes:
* [Create an overlay device tree node](Device%20tree%20overlay.md#create-an-overlay-device-tree-node): [Add an overlay node with label](Device%20tree%20overlay.md#add-an-overlay-node-with-label), [overlay device tree add value to other target](Device%20tree%20overlay.md#overlay-device-tree-add-value-to-other-target) and [add multiple fragments in overlay device tree](Device%20tree%20overlay.md#add-multiple-fragments-in-overlay-device-tree)
* [Read all device tree nodes, included device tree overlay nodes](Device%20tree%20overlay.md#remove-the-inserted-device-tree-overlay-nodes)
* [Remove the inserted device tree overlay nodes](Device%20tree%20overlay.md#remove-the-inserted-device-tree-overlay-nodes)

# Data type

## Array

**Array** in device tree are inside ``<>``, each member is seperated by space. ``,`` is not allowed to used to seperate each member.

E.g: An int array properties inside a devie tree node:

```c
int_array	 = <1 2 3 4>;
```

## [Interrupt](Interrupt.md)

* [Device tree interrupt properties](Interrupt.md#properties): ``interrupts``, ``interrupt-parent``, ``interrupt-cells`` and ``interrupt-controller``
* [Interrupt implementation for platform driver](Interrupt.md#interrupt-implementation-for-platform-driver): [GPIO interrupt](Interrupt.md#gpio-interrupt), [get interrupt number by name](Interrupt.md#get-interrupt-number-by-name)

# Operations for device tree node properties

[Operations for device tree node properties](Operations%20for%20device%20tree%20node%20properties.md) includes:
* [Read device tree node properties](Operations%20for%20device%20tree%20node%20properties.md#read-device-tree-node-properties) by [using platform driver function](Operations%20for%20device%20tree%20node%20properties.md#using-platform-driver-function) and [using overlay device tree function](Operations%20for%20device%20tree%20node%20properties.md#using-overlay-device-tree-function)
* [Create operation for device tree](Operations%20for%20device%20tree%20node%20properties.md#create-operation-for-device-tree): Add a new property to an overlay node
* [Update device tree node properties](Update%20device%20tree%20node%20properties.md) with various types: string, int and array; [update refcount of a device tree node]() by [of_node_get()](API.md#of_node_get) and [of_node_put()](API.md#of_node_put)
* [Delete operation for device tree](Operations%20for%20device%20tree%20node%20properties.md#delete-operation-for-device-tree): Remove property and remove/detach a node
* [Add a new node to device tree by dtsi file modification]()

# [API](API.md)

* [All device tree struct](API.md#struct)
* [of_find_compatible_node()](API.md#of_find_compatible_node)
* [of_find_node_by_type()](API.md#of_find_node_by_type)
* [of_find_property()](API.md#of_find_property)
* [of_property_read_u32_array()](API.md#of_property_read_u32_array)
* [of_node_get()]() and [of_node_put()]()
* [for_each_child_of_node()](API.md#for_each_child_of_node)
