# [Customized Linux distro from scratch](Customized%20Linux%20distro%20from%20scratch)
# [Raspbian booting process](Raspbian%20booting%20process.md)
# [Loadable kernel module](Loadable%20kernel%20module)
* [LKM built on Raspbian]()
* [Cross-compiled LKM]()
* [KConfig](https://github.com/TranPhucVinh/C/blob/master/Kernel/KBuild.md): configs kernel module which stores Raspbian configuration

# [GPIO](GPIO)

# [I2C](I2C)

# [Device tree](Device%20tree)

# Platform driver

## API

### platform_get_irq()

```c
int platform_get_irq(struct platform_device *dev, unsigned int irq_index)
```

* ``irq_index``: Index number of the IRQs inside ``interrupts`` property of the device tree node

E.g

```c
int irq = platform_get_irq(pdev, 0); 
```

This function will return the ``irq`` number; this number is usable by ``devm_request_irq()`` (``irq`` is then visible in ``/proc/interrupts``). The second argument, ``0``, says that we need the first interrupt specified in the device node. If there is more than one interrupt, we can change this index according to the interrupt we need.

## Implementations

* Use platform driver to read properties of an overlay node: [platform_driver_read_dt_node_properties.c](https://github.com/TranPhucVinh/Raspberry-Pi-GNU/blob/main/Kernel/Device%20tree/src/platform_driver_read_dt_node_properties.c)
* Interrupt implementation for platform driver and device tree: [Interrupt](https://github.com/TranPhucVinh/Raspberry-Pi-C/blob/main/Kernel/Device%20tree/Interrupt.md)
