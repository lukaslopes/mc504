#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

#include "hash_ioctl.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
static char hash[MAXSIZE] = "";

char *strcpy(char *dst, const char *src)
{
    while(	((*dst++) = (*src++)));
    return dst;
}

int my_strcmp(char *first, char *second)
{
	while(*first==*second)
   {
      if ( *first == '\0' || *second == '\0' )
         break;
 
      first++;
      second++;
   }
   if( *first == '\0' && *second == '\0' )
      return 0;
   else
      return -1;
}

char *myhash(char *h, const char *str) {
	char *s = h;
	int i = 0;
    while (str[i] != '\0'){
		s[i] = str[i%MAXSIZE] + str[i%MAXSIZE+1];
		s[i] = s[i] % 122;
		if(s[i] < 48)
			s[i] = s[i] + 48;
		i++;
	}
	return s;
}

static int my_open(struct inode *i, struct file *f) {
	return 0;
}
static int my_close(struct inode *i, struct file *f) {
	return 0;
}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	hash_arg_t h;
	char nhash[MAXSIZE] = "";

	switch (cmd) {

		case GET_HASH:
			strcpy(h.hash, hash);
			if (copy_to_user((hash_arg_t *) arg, &h, sizeof(hash_arg_t))) {
				return -EACCES;
			}
		break;
	
		case ERS_HASH:
			strcpy(hash, "");
		break;
	
		case SET_HASH:
			if (copy_from_user(&h, (hash_arg_t *)arg, sizeof(hash_arg_t)))
			{
				return -EACCES;
			}
			myhash(hash, h.hash);
		break;

		case CMP_HASH:
			if (copy_from_user(&h, (hash_arg_t *)arg, sizeof(hash_arg_t)))
			{
				return -EACCES;
			}

			myhash(nhash, h.hash);
			if(my_strcmp(nhash, hash) == 0)
				h.hash_match = 1;
			else
				h.hash_match = -1;

			if (copy_to_user((hash_arg_t *) arg, &h, sizeof(hash_arg_t))) {
				return -EACCES;
			}

		break;
		
		default:
			return -EINVAL;
	}

	return 0;
}

static struct file_operations hash_fops =
// estrutura para chamada do char device
		{ 
			.owner = THIS_MODULE, 
			.open = my_open, 
			.release = my_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	.ioctl = my_ioctl
#else
	.unlocked_ioctl = my_ioctl
#endif
};

static int __init hash_ioctl_init(void)
{
	int ret;
	struct device *dev_ret;

	if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "hash_ioctl")) < 0)
	// aloca o char device na regiao de driver da memoria
	{
		return ret;
	}

	cdev_init(&c_dev, &hash_fops);
	// registra o char device no kernel

	if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
	// informa ao kernel a chamada do char device
	{
		return ret;
	}

	if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
	// criando a device class
	{
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(cl);
	}
	if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "hash")))
	// iniciando o device
	{
		class_destroy(cl);
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}

	return 0;
}

static void __exit hash_ioctl_exit(void)
{
	device_destroy(cl, dev);
	class_destroy(cl);
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, MINOR_CNT);
}

module_init( hash_ioctl_init);
module_exit( hash_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LAL-VRS");
MODULE_DESCRIPTION("hash ioctl() Char Driver");
