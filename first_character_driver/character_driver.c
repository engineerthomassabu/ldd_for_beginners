#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thomas Sabu");
MODULE_DESCRIPTION("My first Character Device Driver");

#define DEVICE_NAME "my_character_device"

static dev_t dev_num;          // Major and minor numbers
static struct cdev my_cdev;    // Character device structure
static char kernel_buffer[256]; // Data buffer

static int my_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "my_character_device: Opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "my_character_device: Closed\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    int bytes_read = simple_read_from_buffer(buf, len, offset, kernel_buffer, strlen(kernel_buffer));
    printk(KERN_INFO "my_character_device: Read %d bytes\n", bytes_read);
    return bytes_read;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    int bytes_written = simple_write_to_buffer(kernel_buffer, sizeof(kernel_buffer), offset, buf, len);
    kernel_buffer[bytes_written] = '\0';
    printk(KERN_INFO "my_character_device: Written %d bytes: %s\n", bytes_written, kernel_buffer);
    return bytes_written;
}


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

static int __init my_character_device_init(void)
{
    // 1. Allocate device number
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    printk(KERN_INFO "my_character_device: Major = %d, Minor = %d\n", MAJOR(dev_num), MINOR(dev_num));

    // 2. Initialize cdev
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    // 3. Add to kernel
    cdev_add(&my_cdev, dev_num, 1);

    printk(KERN_INFO "my_character_device: Driver loaded successfully\n");
    return 0;
}

static void __exit my_character_device_exit(void)
{
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "my_character_device: Driver unloaded\n");
}

module_init(my_character_device_init);
module_exit(my_character_device_exit);


