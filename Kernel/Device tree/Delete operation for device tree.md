# Remove property

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

# Remove/detach a node

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
# Remove a property of a node and add a new empty property (i.e property without any value)

Program: [remove_and_add_a_new_node.c](remove_and_add_a_new_node.c)

Original node:

```
gpio-restart {
        gpios = <0x4 0xf 0x1>;
        compatible = "gpio-restart";
        priority = <0xc8>;
};
```
Node after running [remove_and_add_a_new_node.c](remove_and_add_a_new_node.c):
```
gpio-restart {
		gpios = <0x4 0xf 0x1>;
		compatible = "gpio-restart";
		new_priority;
};
```
