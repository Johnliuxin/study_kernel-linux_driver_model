#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/slab.h>		/*kzalloc, kmalloc*/
#include <linux/sysfs.h>	/*optional: has been included in kobject.h */
#include <linux/platform_device.h>   /*platform related*/
#include <linux/err.h>		/*IS_ERR, ERR_PTR*/

/* 
 * Another special macro (MODULE_LICENSE) is used to tell the kernel that this 
 * module bears a free license; without such a declaration, the kernel 
 * complains when the module is loaded.
 */
MODULE_LICENSE("Dual BSD/GPL");

/*
 * platform device related code
 ******************************************************************************
 */
static struct platform_device *embest_platform_device;

/*
 * platform driver related code
 ******************************************************************************
 */
static int embest_platform_driver_probe(struct platform_device *pdev)
{
	printk(KERN_ALERT "embest platform driver probe, device name %s \n", dev_name(&pdev->dev));
	
	return 0;
}

static int embest_platform_driver_remove(struct platform_device *pdev)
{
	printk(KERN_ALERT "embest platform driver remove, device name %s \n", dev_name(&pdev->dev));
	
	return 0;
}

static struct platform_driver embest_platform_driver = {
	.probe = embest_platform_driver_probe,
	.remove = embest_platform_driver_remove,
	.driver = {
		.name = "embestPlatformDev",
		.owner		= THIS_MODULE,
	},
};

static int __init example_init(void)
{
	int error;
	
	embest_platform_device = platform_device_register_data(NULL, 
	                                                       "embestPlatformDev",
														   PLATFORM_DEVID_AUTO,
														   NULL,
														   0);
	if (IS_ERR(embest_platform_device)) {
		printk(KERN_ALERT "create embest platform device error\n");
		return -ENODEV;
	}
	
	error = platform_driver_register(&embest_platform_driver);
	if (error) {
		printk(KERN_ALERT "create embest platform driver error\n");
		platform_device_unregister(embest_platform_device);
		return error;
	}
	
	return 0;
}

static void example_exit(void)
{
	platform_driver_unregister(&embest_platform_driver);
	platform_device_unregister(embest_platform_device);
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of platform device/driver");