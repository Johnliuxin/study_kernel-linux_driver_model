#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/slab.h>		/*kzalloc, kmalloc*/
#include <linux/sysfs.h>	/*optional: has been included in kobject.h */
#include <linux/device.h>   /*bus, device, device_driver*/

/* 
 * Another special macro (MODULE_LICENSE) is used to tell the kernel that this 
 * module bears a free license; without such a declaration, the kernel 
 * complains when the module is loaded.
 */
MODULE_LICENSE("Dual BSD/GPL");

/*
 * Bus related code
 ******************************************************************************
 */
static int bus_attr_value = 0;

ssize_t bus_attr_store(struct bus_type *bus,
			       const char *buf, size_t count)
{
	ssize_t ret = -EIO;
	
	sscanf(buf, "%du", &bus_attr_value);
	
	printk(KERN_ALERT "bus attribute value from user %s\n", buf);
	
	ret = count;
	
	return ret;
}

ssize_t bus_attr_show(struct bus_type *bus, char *buf)
{
	ssize_t ret = -EIO;
	
	ret = sprintf(buf, "%d\n", bus_attr_value);
	
	return ret;
}

/*the attribute for the bus*/
static BUS_ATTR(embus_attr, 0664, bus_attr_show, bus_attr_store);

static struct attribute *attrs[] = {
	&bus_attr_embus_attr.attr,
	NULL,
};

static struct attribute_group bus_attr_group = {
	.attrs = attrs,
};

const struct attribute_group *bus_attr_groups[] = {
	&bus_attr_group,
	NULL,
};

struct bus_type embest_bus = {
	.name = "embestBus",
	.bus_groups = bus_attr_groups,
};

/*
 * Device related code
 ******************************************************************************
 */
static struct device embest_device = {
	.init_name = "embestDev",
	.bus = &embest_bus,
	//no release function, when do device_unregister, kernel will give a WARNING
	//but it don't affect
};

 /*
 * Driver related code
 ******************************************************************************
 */
 
static int __init example_init(void)
{
	int error;
	
	error =  bus_register(&embest_bus);
	if (error) {
		printk(KERN_ALERT "embest bus register failed \n");
		goto bus_register_fail;
	}
	
	error = device_register(&embest_device);
	if (error) {
		printk(KERN_ALERT "embest device register failed \n");
		goto device_register_fail;
	}

	return 0;

device_register_fail:
	bus_unregister(&embest_bus);
bus_register_fail:
	return error;
}

static void example_exit(void)
{
	device_unregister(&embest_device);
	bus_unregister(&embest_bus);
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of bus, device, device_driver");