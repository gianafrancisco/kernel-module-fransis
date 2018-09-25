#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include "fransis.h"

static dev_t fransis_dev;
int _fransis_g_major = 0;
static struct cdev *fransis_cdev = NULL;
static struct class *cl;



long fransis_ioctl (
	struct file *p_file,
	unsigned int cmd,
	unsigned long buf)
{
    printk("fransis: ioctl\n");
    return 0;
}

long fransis_ioctl_32 (
	struct file *p_file,
	unsigned int cmd,
	unsigned long buf)
{
    printk("fransis: ioctl 32 bit\n");
    return 0;
}

int fransis_open (
	struct inode *p_inode,
	struct file *p_file)
{
    printk("fransis: open\n");
    return 0;
}

int fransis_release (
	struct inode *p_inode,
	struct file *p_file)
{
    printk("fransis: release\n");
    return 0;
}

ssize_t fransis_write(struct file *f, const char __user *buf, size_t len,
    loff_t *off)
{
    printk("fransis: write\n");
    return len;
}

ssize_t fransis_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk("fransis: read\n");
    return 0;
}

struct file_operations fransis_fops = {
    .owner = THIS_MODULE,
    .open = fransis_open,
		.read = fransis_read,
		.write = fransis_write,
    .release = fransis_release,
    .unlocked_ioctl = fransis_ioctl,
		.compat_ioctl = fransis_ioctl_32
};

int fransis_create_and_register_device (void)
{
    int result = 0;
    int ret = 0;
    int num_fransis_devices = 1;
    struct device *dev_ret;

    ret = alloc_chrdev_region (&fransis_dev, 0, 1,
	                       FRANSIS_DEV_BASE_NAME);

    if (ret < 0)
    {
			printk ("fransis: Failed to register with kernel. err = %d.\n", ret);
			return ret;
    }

    if (IS_ERR(cl = class_create(THIS_MODULE, FRANSIS_DEV_BASE_NAME)))
    {
        unregister_chrdev_region(fransis_dev, 1);
        return PTR_ERR(cl);
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, fransis_dev, NULL, FRANSIS_DEV_BASE_NAME)))
    {
        class_destroy(cl);
        unregister_chrdev_region(fransis_dev, 1);
        return PTR_ERR(dev_ret);
    }

    _fransis_g_major = MAJOR(fransis_dev);

    printk ("fransis: major no is: %d\n", _fransis_g_major);

    fransis_cdev = cdev_alloc();
    if (!fransis_cdev)
    {
        printk ("fransis: failed to alloc cdev\n");
        result = 1;
        goto out;
    }

    cdev_init(fransis_cdev, &fransis_fops);
    fransis_cdev->owner = THIS_MODULE;

    result = cdev_add(fransis_cdev, fransis_dev, num_fransis_devices);
    if (result != 0)
    {
			printk ("fransis: Failed to cdev_add %d devices\n", num_fransis_devices);
				goto out;
    }

    if (!try_module_get (THIS_MODULE))
    {
	     printk ("fransis: Failed to reference device. try_module_get failed.\n");
    }

out:
    if (result != 0)
    {
				if (fransis_cdev)
					cdev_del(fransis_cdev);
				device_destroy(cl, fransis_dev);
				class_destroy(cl);
				unregister_chrdev_region (fransis_dev, num_fransis_devices);
    }

    return result;
}


static int __init fransis_init(void) /* Constructor */
{
    printk("fransis: registered\n");
    fransis_create_and_register_device();
    printk("fransis: registered device fransis\n");
    return 0;
}

static void __exit fransis_exit(void) /* Destructor */
{
    cdev_del(fransis_cdev);
    device_destroy(cl, fransis_dev);
    class_destroy(cl);
    unregister_chrdev_region(fransis_dev, 1);
    printk("fransis: unregistered\n");
}

module_init(fransis_init);
module_exit(fransis_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francisco Giana <gianafrancisco@gmail.com>");
MODULE_DESCRIPTION("My First Driver");
