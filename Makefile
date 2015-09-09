# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
    obj-m := manual_kobject_attribute.o
    obj-m += auto_kobject_attribute.o
    obj-m += manual_kset_kobject_attribute.o
    obj-m += bus_device_driver.o 
    obj-m += class_device.o
    obj-m += platform_device_driver.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else

    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    PWD  := $(shell pwd)

default:    
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

clean:
	rm -f *.o *.ko

distclean:
	rm -f *.o *.ko *.mod.c modules.* Module.*
