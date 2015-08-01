#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/slab.h>		/*kzalloc, kmalloc*/
#include <linux/sysfs.h>	/*optional: has been included in kobject.h */

/* 
 * Another special macro (MODULE_LICENSE) is used to tell the kernel that this 
 * module bears a free license; without such a declaration, the kernel 
 * complains when the module is loaded.
 */
MODULE_LICENSE("Dual BSD/GPL");

static struct kobject *example_kobj;

static int kobj_attr_value = 0;

/*the attribute for the kobject*/
static struct attribute kobj_attr = {
	.name = "kobj_attr",
	.mode = VERIFY_OCTAL_PERMISSIONS(0664),
};

static void ktype_release(struct kobject *kobj)
{
	printk(KERN_ALERT "release kobject (%p)\n", kobj);
	kfree(kobj);
}

static ssize_t kobject_attr_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	ssize_t ret = -EIO;
	
	ret = sprintf(buf, "%d\n", kobj_attr_value);
	
	return ret;
}

static ssize_t kobject_attr_store(struct kobject *kobj, struct attribute *attr,
			       const char *buf, size_t count)
{
	ssize_t ret = -EIO;
	
	sscanf(buf, "%du", &kobj_attr_value);
	
	printk(KERN_ALERT "attribute value from user %s\n", buf);

	ret = count;
	
	return ret;
}

const struct sysfs_ops kobject_sysfs_ops = {
	.show	= kobject_attr_show,
	.store	= kobject_attr_store,
};

/*your own ktype for the kobject*/
static struct kobj_type ktype = {
	.release = ktype_release,
	.sysfs_ops = &kobject_sysfs_ops,
};

static int __init example_init(void)
{
	int retval;
	
	/*first: allocate a kobject memory*/
	example_kobj = kzalloc(sizeof(*example_kobj), GFP_KERNEL);
	if (!example_kobj)
		return -ENOMEM;
	
	/*second: define your own ktype, and init the kobject*/
	kobject_init(example_kobj, &ktype);
	
	/*third: add the kobject to kernel*/
	retval = kobject_add(example_kobj, NULL, "%s", "example_kobj");
	if (retval) {
		printk(KERN_WARNING "%s: kobject_add error: %d\n",
		       __func__, retval);
		goto kobject_add_error;
	}
	
	/*forth: create the attribute file associated with this kobject */
	retval = sysfs_create_file(example_kobj, &kobj_attr);
	if (retval) {
		printk(KERN_WARNING "%s: sysfs_create_file error: %d\n",
		       __func__, retval);
		goto create_attribute_error;
	}
	
	return 0;

kobject_add_error:
create_attribute_error:
	kobject_put(example_kobj);
	example_kobj = NULL;
	return retval;
}

static void example_exit(void)
{
	kobject_put(example_kobj);
	example_kobj = NULL;
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of manual create kobject and attribute");