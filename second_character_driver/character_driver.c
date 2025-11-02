#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thomas Sabu");
MODULE_DESCRIPTION("Character Device Driver code with autyo device node creation");

#define DEVICE_NAME "myclassdev"
#define BUFFER_SIZE 256


static dev_t dev_num;          // Major and minor numbers
static struct cdev my_cdev;    // Character device structure
static char kernel_buffer[BUFFER_SIZE]; // Data buffer
static ssize_t data_size =0;
static struct class *my_class;

static int my_open(struct inode *inode, struct file *file)
{
	pr_info ("character device file opened \n");  // pr_info is printk() with the KERN_INFO priority
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info("character_device: Closed\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{

//	size_t bytes_read;

    	if (*offset >= data_size)
        	return 0; // EOF

	if (len > data_size - *offset)
        	len = data_size - *offset;

	if (copy_to_user(buf, kernel_buffer + *offset, len))
        	return -EFAULT;

   	*offset += len;
   	pr_info("myclassdev: Read %zu bytes\n", len);
	return len;
}
	


static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
	if (len > BUFFER_SIZE)
        len = BUFFER_SIZE;

	if (copy_from_user(kernel_buffer, buf, len))
        	return -EFAULT;

	data_size = len;
	kernel_buffer[len] = '\0';

	pr_info("myclassdev: Written %zu bytes: %s\n", len, kernel_buffer);
    	return len;	

}
static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.read = my_read,
	.write = my_write,
};


static int __init my_cdevice_init(void)
{
	int ret;

    	/* Step 1: Allocate device number */
    	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    	if (ret < 0) 
	{
        	pr_err("%s: Failed to allocate device number (%d)\n", DEVICE_NAME, ret);
        	return ret;
   	}

	 /* Step 2: Initialize and add cdev */
	cdev_init(&my_cdev, &fops);
   	my_cdev.owner = THIS_MODULE;

   	ret = cdev_add(&my_cdev, dev_num, 1);
   	if (ret < 0) 
	{
        	pr_err("%s: Failed to add cdev (%d)\n", DEVICE_NAME, ret);
        	unregister_chrdev_region(dev_num, 1);
        	return ret;
    	}

	/* Step 4: Create class */
	my_class = class_create("myclass");
    	if (IS_ERR(my_class)) 
	{
        	ret = PTR_ERR(my_class);
        	pr_err("%s: Failed to create class (%d)\n", DEVICE_NAME, ret);  //pr_err is printk() with KERN_ERR priority
        	cdev_del(&my_cdev);
        	unregister_chrdev_region(dev_num, 1);
        	return ret;
    	}

	/* Step 4: Create device node */
	if (IS_ERR(device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME))) 
	{
        	pr_err("%s: Failed to create device node\n", DEVICE_NAME); 
 		class_destroy(my_class);
        	cdev_del(&my_cdev);
        	unregister_chrdev_region(dev_num, 1);
        	return -1;
	}

    	pr_info("%s: Driver loaded successfully\n", DEVICE_NAME);
    	pr_info("Device created: /dev/%s (Major %d Minor %d)\n",DEVICE_NAME, MAJOR(dev_num), MINOR(dev_num));
    	return 0;
}

static void __exit my_cdevice_exit(void)
{
	device_destroy(my_class, dev_num);
	class_destroy(my_class);
	cdev_del(&my_cdev);
    	unregister_chrdev_region(dev_num, 1);
    	pr_info("%s: Driver unloaded\n", DEVICE_NAME);
    	cdev_del(&my_cdev);
    	unregister_chrdev_region(dev_num, 1);
    	printk(KERN_INFO "my_character_device: Driver unloaded\n");
}

module_init(my_cdevice_init);
module_exit(my_cdevice_exit);


