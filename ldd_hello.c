#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("THOMAS SABU");
MODULE_DESCRIPTION ("FIRST SIMPLE DRIVER FOR BEGINNER TUTORIAL");
MODULE_VERSION("1.1.0");

static int __init hello_init(void)
{
	printk("loaded the module to kernel \n");
	return 0;
}

static void  __exit hello_exit(void)
{
	printk("unloaded the module from kernel \n");
}


module_init(hello_init);
module_exit(hello_exit);


