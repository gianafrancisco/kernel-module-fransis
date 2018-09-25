#ifndef FRANSIS_H
#define FRANSIS_H

#include <linux/ioctl.h>

#define FRANSIS_DEV_BASE_NAME "fransis"
#define FRANSIS_DEV_BASE_DIR_NAME "/dev/"
#define FRANSIS_DEVICE_MINOR  0
#define FRANSIS_DEV_NAME  "/dev/fransis"
#define DEVICE_FILE_NAME "/dev/fransis"

#define	VFS_UNLINK(inode,dentry,vfsmnt)		vfs_unlink((inode),(dentry), NULL)
#define	VFS_MKNOD(inode,dentry,vfsmnt,dev)	vfs_mknod ((inode),(dentry),S_IFCHR | 0700, (dev))
#define PATH_RELEASE(nd)                path_put(&(nd)->path)

#define MAJOR_NUM 240

#define IOCTL_SET_MSG _IOR(MAJOR_NUM, 0, char *)

#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, char *)

#define IOCTL_GET_NTH_BYTE _IOWR(MAJOR_NUM, 2, int)




#endif
