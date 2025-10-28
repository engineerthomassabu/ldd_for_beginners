#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("THOMAS SABU");
MODULE_DESCRIPTION ("BEGINNER TUTORIAL TO UNDERSTAND VIRTUAL FILE STYSEM (PROC)");
MODULE_VERSION("1.1.0");


static struct proc_dir_entry *hello_driver_proc_entry;

static ssize_t hello_driver_read (struct file * file_pointer,
	               	char *user_space_buffer, 
			size_t count, 
			loff_t* offset) 
{
	char msg[] ="did a proc read. Acknowledged\n";
	ssize_t length =strlen(msg);
	int result;
	
	printk(" hello_driver proc read_function \n");

	if(*offset >= length)
	{
		return 0;
	}

	result = copy_to_user (user_space_buffer, msg, length);

	*offset += length;
	return length;
}


struct proc_ops hello_driver_proc_ops =
{ 
	.proc_read =hello_driver_read					
};

static int __init hello_driver_init(void)
{
	printk("hello_driver loading to kernel started \n ");

	hello_driver_proc_entry = proc_create("hello_driver_proc_read",

               			0, 
				NULL, 
				&hello_driver_proc_ops);

	printk("hello_driver loaded to kernel and driver entry done to /proc \n");

	return 0;

}

static void  __exit hello_driver_exit(void)
{
	printk("unloading hello_driver from kernel started \n");

	proc_remove(hello_driver_proc_entry);

	printk("hello_driver unloaded from kernel and removed the  /proc entry \n");
}


module_init(hello_driver_init);
module_exit(hello_driver_exit);


