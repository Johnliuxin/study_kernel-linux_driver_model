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

static struct kset    *example_kset;
static struct kobject *example_kobj;

static int kset_attr_value = 0;
static int kobj_attr_value = 0;

/*
 * functions for kset
 */

/*the attribute for the kset*/
static struct attribute kset_attr = {
	.name = "kset_attr",
	.mode = VERIFY_OCTAL_PERMISSIONS(0664),
};

static void kset_self_release(struct kobject *kobj)
{
	struct kset *kset = container_of(kobj, struct kset, kobj);
	printk(KERN_ALERT "release kset (%p)\n", kset);
	kfree(kset);
}

static ssize_t kset_kobj_attr_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	ssize_t ret = -EIO;
	
	ret = sprintf(buf, "%d\n", kset_attr_value);
	
	return ret;
}

static ssize_t kset_kobj_attr_store(struct kobject *kobj, struct attribute *attr,
			       const char *buf, size_t count)
{
	ssize_t ret = -EIO;
	
	sscanf(buf, "%du", &kset_attr_value);
	
	printk(KERN_ALERT "attribute value from user for kset %s\n", buf);

	ret = count;
	
	return ret;
}

const struct sysfs_ops kset_kobj_sysfs_ops = {
	.show	= kset_kobj_attr_show,
	.store	= kset_kobj_attr_store,
};

/*your own ktype for the kset's kobject*/
static struct kobj_type kset_self_ktype = {
	.release = kset_self_release,
	.sysfs_ops = &kset_kobj_sysfs_ops,
};

/*
 * functions for kobject
 */
 
static ssize_t kobject_attr_show(struct kobject *kobj, struct kobj_attribute *attr,
			      char *buf)
{
	ssize_t ret = -EIO;
	
	ret = sprintf(buf, "%d\n", kobj_attr_value);
	
	return ret;
}

static ssize_t kobject_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			       const char *buf, size_t count)
{
	ssize_t ret = -EIO;
	
	sscanf(buf, "%du", &kobj_attr_value);
	
	printk(KERN_ALERT "attribute value from user for kobject %s\n", buf);

	ret = count;
	
	return ret;
}

/*the attribute for the kobject*/
static struct kobj_attribute kobj_attr =
	__ATTR(kobj_attr, 0664, kobject_attr_show, kobject_attr_store);

static void ktype_release(struct kobject *kobj)
{
	printk(KERN_ALERT "release kobject (%p)\n", kobj);
	kfree(kobj);
}

/*your own ktype for the kobject*/
static struct kobj_type kobject_ktype = {
	.release	= ktype_release,
	/*Note: 
	 * Here we don't define the ops but use kobj_sysfs_ops which is defined in kobject.c
	 * because we have done it in manual_kobject_attribute, don't want to do it again
	 */
	.sysfs_ops	= &kobj_sysfs_ops,
};

static int __init example_init(void)
{
	int retval;
	
	/*first: allocate a kset memory and prepare the kobj.ktype for this kset*/
	example_kset = kzalloc(sizeof(*example_kset), GFP_KERNEL);
	if(!example_kset)
		return -ENOMEM;
	retval = kobject_set_name(&example_kset->kobj, "%s", "example_kset");
	if (retval) {
		kfree(example_kset);
		return retval;
	}
	example_kset->uevent_ops = NULL;
	example_kset->kobj.parent = NULL;
	example_kset->kobj.ktype = &kset_self_ktype;
	example_kset->kobj.kset = NULL;
	
	/*second: register the kset*/
	retval = kset_register(example_kset);
	if (retval) {
		kfree(example_kset);
		return retval;
	}
	
	/*third: create the attribute file associated with this kset*/
	retval = sysfs_create_file(&example_kset->kobj, &kset_attr);
	if (retval) {
		printk(KERN_WARNING "%s: sysfs_create_file for kset error: %d\n",
		       __func__, retval);
		goto create_kset_attribute_error;
	}
	
	/*4th: allocate a kobject memory*/
	example_kobj = kzalloc(sizeof(*example_kobj), GFP_KERNEL);
	if (!example_kobj) {
		retval = -ENOMEM;
		goto allocate_kobject_error;
	}
	
	/*5th: define your own ktype, and init the kobject*/
	kobject_init(example_kobj, &kobject_ktype);
	
	/*6th: set the kobject's kset*/
	example_kobj->kset = example_kset;
	
	/*7th: add the kobject to kernel*/
	retval = kobject_add(example_kobj, NULL, "%s", "example_kobj");
	if (retval) {
		printk(KERN_WARNING "%s: kobject_add error: %d\n",
		       __func__, retval);
		goto kobject_add_error;
	}
	
	/*8th: create the attribute file associated with this kobject */
	retval = sysfs_create_file(example_kobj, &kobj_attr.attr);
	if (retval) {
		printk(KERN_WARNING "%s: sysfs_create_file error: %d\n",
		       __func__, retval);
		goto create_attribute_error;
	}
	
	return 0;

create_attribute_error:
kobject_add_error:
	kobject_put(example_kobj);
allocate_kobject_error:
	example_kobj = NULL;
create_kset_attribute_error:
	kset_unregister(example_kset);
	example_kset = NULL;
	return retval;
}

static void example_exit(void)
{
	kobject_put(example_kobj);
	example_kobj = NULL;
	kset_unregister(example_kset);
	example_kset = NULL;
}

module_init(example_init);
module_exit(example_exit);

MODULE_AUTHOR("John LiuXin");
MODULE_DESCRIPTION("Example of manual create kobject and attribute");