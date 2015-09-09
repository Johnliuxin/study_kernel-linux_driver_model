#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/slab.h>		/*kzalloc, kmalloc*/
#include <linux/sysfs.h>	/*optional: has been included in kobject.h */
#include <linux/device.h>   /*device, class*/
#include <linux/err.h>		/*IS_ERR, ERR_PTR*/

/* 
 * Another special macro (MODULE_LICENSE) is used to tell the kernel that this 
 * module bears a free license; without such a declaration, the kernel 
 * complains when the module is loaded.
 */
MODULE_LICENSE("Dual BSD/GPL");

/*
 * Class related code
 ******************************************************************************
 */
static struct class *embest_class;

/*
 * Device related code
 ******************************************************************************
 */
static struct device *embest_device;

static int __init example_init(void)
{
	embest_class = class_create(THIS_MODULE, "embestClass");
	if (IS_ERR(embest_class)) {
		printk(KERN_ALERT "create embest class error\n");
		return -ENODEV;
	}
	
	embest_device = device_create(embest_class, NULL, 0, NULL, "embestDev");
	if (IS_ERR(embest_device)) {
		printk(KERN_ALERT "create embest device error\n");
		return -ENODEV;
	}
	
	return 0;

}

static void example_exit(void)
{
	device_destroy(embest_class, 0);
	class_destroy(embest_class);
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of class, device");