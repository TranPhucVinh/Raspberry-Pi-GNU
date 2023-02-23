**General information**: For safety purpose, we will change the property of the added overlay device tree so that no built-in device tree properties are effected.

# Read device tree node properties

Read properties of an overlay node.

Overlay node:

```sh
/dts-v1/;
/plugin/;
/ {
	compatible = "brcm,bcm2835";
	fragment@0 {
        target-path = "/";
		__overlay__ {
			new_dt_node {
				compatible = "compatible_string";
                                device_type = "new_dt_node";
                                string_val_1 = "String value 1";
                                string_val_2 = "String value 2";
                                int_val_1    = <123>;
				int_array	 = <1 2 3 4>;
			};
		};
	};
};
```

## Using platform driver function

With device tree node ``new_dt_node`` is inserted in the device tree, ``probe()`` function in platform driver is called.

``compatible_string`` in ``compatible`` is used by ``struct of_device_id``.

Running ``rmmod`` will call platform driver ``remove()`` function.

Read all properties of ``new_dt_node``: [read_device_tree_node_properties.c](read_device_tree_node_properties.c)

## Using overlay device tree function

Node functions like [of_find_node_by_type()](https://github.com/TranPhucVinh/Raspberry-Pi-C/tree/main/Kernel/Device%20tree#of_find_node_by_type), of_find_compatible_node(),... support reading nodes of device tree.

Property functions like [of_find_property()](https://github.com/TranPhucVinh/Raspberry-Pi-C/tree/main/Kernel/Device%20tree#of_find_property), [of_property_read_u32_array()](https://github.com/TranPhucVinh/Raspberry-Pi-C/tree/main/Kernel/Device%20tree#of_property_read_u32_array),... will support reading properties values inside a specific node.

# Create operation for device tree

Add a new property to an overlay node.

```c
#define COMPATIBLE    	"compatible_string"
#define PROP_VAL     	"new_property"
#define ADD_VAL   		"new_property_string"

int add_node_properties(struct of_changeset *ocs, const char* compatible, const char* property_name, const char *value){
	struct device_node  *dev_node;
	struct property  	*prop;

	dev_node = of_find_compatible_node(NULL, NULL, compatible);
	
	of_changeset_init(ocs);//Init of_changset objects

	prop = kmalloc(sizeof(struct property), GFP_KERNEL);
	prop->name = kstrdup(property_name, GFP_KERNEL);//property name
	prop->value = kstrdup(value, GFP_KERNEL);//property value
	prop->length = strlen(value) + 1;

	of_changeset_add_property(ocs, dev_node, prop);
	
	mutex_lock(&of_mutex);
	int ret = of_changeset_apply(ocs);
	if(ret < 0) {
		printk("Unable to apply changset, ret: %d\n", ret);
		return -1;
	}
	mutex_unlock(&of_mutex);

	return 0; 
}

int init_module(void)
{
	mutex_init(&of_mutex);
    ocs = kmalloc(sizeof(struct of_changeset), GFP_KERNEL);
	add_node_properties(ocs, COMPATIBLE, PROP_VAL, ADD_VAL);	
	return 0;
}
```

# [Update device tree node properties]()

# Delete operation for device tree

## Remove property

Remove property with ``of_changeset_remove_property()``:

```c
#define COMPATIBLE    	"compatible_string"
#define PROP_VAL     	"string_val_1"

int remove_node_properties(struct of_changeset *ocs, const char* compatible, const char* property_name){
	struct device_node  *dev_node;
	struct property  	*prop;

	dev_node = of_find_compatible_node(NULL, NULL, compatible);
	prop = of_find_property(dev_node, PROP_VAL, NULL);

	of_changeset_init(ocs);//Init of_changset objects

	of_changeset_remove_property(ocs, dev_node, prop);
	
	mutex_lock(&of_mutex);
	int ret = of_changeset_apply(ocs);
	if(ret < 0) {
		printk("Unable to update device tree property, ret: %d\n", ret);
		return -1;
	}
	mutex_unlock(&of_mutex);

	return 0; 
}

int init_module(void)
{
	mutex_init(&of_mutex);
    ocs = kmalloc(sizeof(struct of_changeset), GFP_KERNEL);
	remove_node_properties(ocs, COMPATIBLE, PROP_VAL);	
	return 0;
}
```

``of_changeset_remove_property()`` only work if its ``prop`` argument is valid setup. So it's best to use ``of_find_property()`` to return a valid ``struct property`` object for used in ``of_changeset_remove_property()``.

If setting up ``struct property`` with ``name``, ``value`` and ``length`` like this (omit other values), ``of_changeset_remove_property()`` will fail:

```c
//of_changeset_remove_property() will fail with this setup
prop = kmalloc(sizeof(struct property), GFP_KERNEL);
prop->name = kstrdup(property_name, GFP_KERNEL);//property name
prop->value = kstrdup(value, GFP_KERNEL);//property value
prop->length = strlen(value) + 1;

of_changeset_init(ocs);//Init of_changset objects

of_changeset_remove_property(ocs, dev_node, prop);
```

## Remove/detach a node

```c
#define COMPATIBLE    	"compatible_string"

int detach_a_node(struct of_changeset *ocs, const char* compatible){
	struct device_node  *detached_node;

	detached_node = of_find_compatible_node(NULL, NULL, compatible);

	of_changeset_init(ocs);

	int ret = of_changeset_detach_node(ocs, detached_node);
	if(ret < 0) {
		printk("Unable to detach node %s, ret: %d\n", compatible, ret);
		return -1;
	} else printk("Detach node %s successfully\n", compatible);
	
	mutex_lock(&of_mutex);
	ret = of_changeset_apply(ocs);
	if(ret < 0) {
		printk("Unable to apply changset, ret: %d\n", ret);
		return -1;
	}
	mutex_unlock(&of_mutex);

	return 0; 
}

int init_module(void)
{
	mutex_init(&of_mutex);
    ocs = kmalloc(sizeof(struct of_changeset), GFP_KERNEL);
	detach_a_node(ocs, COMPATIBLE);	
	return 0;
}
```

**Note**: A node can only be removed whenever there is no kernel module accessing to it.
