#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/namei.h>
#include <asm/uaccess.h>

#define FRANSIS_DEV_BASE_NAME "fransis"
#define FRANSIS_DEV_BASE_DIR_NAME "/dev/"
#define FRANSIS_DEVICE_MINOR  0
#define FRANSIS_DEV_NAME  "/dev/fransis"

#define	VFS_UNLINK(inode,dentry,vfsmnt)		vfs_unlink((inode),(dentry), NULL)
#define	VFS_MKNOD(inode,dentry,vfsmnt,dev)	vfs_mknod ((inode),(dentry),S_IFCHR | 0700, (dev))
#define PATH_RELEASE(nd)                path_put(&(nd)->path)

static dev_t fransis_dev;
int _fransis_g_major = 0;
static struct cdev *fransis_cdev = NULL;
static struct class *cl;



long fransis_ioctl (
	struct file *p_file,
	unsigned int cmd,
	unsigned long buf)
{
    printk("fransis: ioctl");
    return 0;
}

int fransis_open (
	struct inode *p_inode,
	struct file *p_file)
{
    printk("fransis: open");
    return 0;
}

int fransis_release (
	struct inode *p_inode,
	struct file *p_file)
{
    printk("fransis: release");
    return 0;
}


struct file_operations fransis_fops = {
    .owner = THIS_MODULE,
    .open = fransis_open,
    .release = fransis_release,
    .unlocked_ioctl = fransis_ioctl
};

int fransis_create_devnode (char *dev_base_name, char *dev_base_name_dir,
                          int major, int start_minor, int count)
{
/*  mm_segment_t oldfs = get_fs();
  struct dentry *p_dentry = NULL;
  struct nameidata nd = {.flags = 0};
  int i = 0;
  char dev_name[] = FRANSIS_DEV_NAME;
  long error = 0;
  dev_t dev = 0;
  struct path path = {0};
  struct filename fname = {0};
  fname.name = dev_name;
  nd.name = &fname;
  nd.dfd = AT_FDCWD;
  error = path_parentat (&nd,  LOOKUP_RCU, &path);
  nd.path = path;
  if (error != 0)
  {
    printk ("fransis: path_lookup for '%s' failed. err = %ld.\n",
      dev_name, error);
    goto out;
  }
      dev = MKDEV(major, i);
      set_fs (KERNEL_DS);

      error = VFS_MKNOD(nd.DENTRY->d_inode,p_dentry,nd.MNT,dev);
      if (error == -EEXIST)
      {
          error = VFS_UNLINK(nd.DENTRY->d_inode, p_dentry,nd.MNT);
          if (error)
          {
            printk ("Failed to remove the device %s . err = %d\n",dev_name, (int)error);
              error = 0;
          }
          else
          {
            error = VFS_MKNOD(nd.DENTRY->d_inode,p_dentry,nd.MNT,dev);
            if (!error)
                printk ("Successfully removed the stale dev node %s and recreated it.\n", dev_name);
            else
              printk ("Successfully removed the stale dev node %s but failed recreating it with ret %d.\n", dev_name, (int)error);
          }
      }

      set_fs (oldfs);
      inode_unlock(nd.DENTRY->d_inode);
      PATH_RELEASE(&nd);
  out:
    return error;*/
    return 0;
}

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
/*
    result = fransis_create_devnode (FRANSIS_DEV_BASE_NAME, FRANSIS_DEV_BASE_DIR_NAME,
                                    _fransis_g_major, FRANSIS_DEVICE_MINOR, num_fransis_devices);
    if (result != 0)
    {
	printk ("fransis: Failed to create the device node `%s'. err = %d.\n",
	       FRANSIS_DEV_NAME, result);
	goto out;
    }
*/
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
    printk("fransis: registered");
    fransis_create_and_register_device();
    printk("fransis: registered device fransis");
    return 0;
}

static void __exit fransis_exit(void) /* Destructor */
{
    cdev_del(fransis_cdev);
    device_destroy(cl, fransis_dev);
    class_destroy(cl);
    unregister_chrdev_region(fransis_dev, 1);
    printk("fransis: unregistered");
}

module_init(fransis_init);
module_exit(fransis_exit);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francisco Giana <gianafrancisco@gmail.com>");
MODULE_DESCRIPTION("My First Driver");
