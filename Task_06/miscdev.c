#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

static const char id[] = "716e82d712cbb";

static ssize_t misc_read(struct file *filp,
		char *buffer,
		size_t len,
		loff_t *offset);
static ssize_t misc_write(struct file *filp,
		const char *buffer,
		size_t len,
		loff_t *offset);

const struct file_operations misc_fops = {
	.owner = THIS_MODULE,
	.read = misc_read,
	.write = misc_write
};

static struct miscdevice miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &misc_fops
};

static ssize_t misc_read(struct file *filp,
		char *buffer,
		size_t len,
		loff_t *offset)
{
	return simple_read_from_buffer(buffer, len, offset, id, sizeof(id));
}
static ssize_t misc_write(struct file *filp,
		const char *buffer,
		size_t len,
		loff_t *offset)
{
	ssize_t ret;

	/* Allocate 1 more byte in case buffer is longer than ID length */
	char *id_buffer = kcalloc(1, sizeof(id) + 1, GFP_KERNEL);

	if ((ret = simple_write_to_buffer(id_buffer,
				sizeof(id), offset, buffer, len - 1)) < 0) {
		kfree(id_buffer);
		return ret;
	}

	if (strncmp(id, id_buffer, sizeof(id)) != 0) {
		kfree(id_buffer);
		return -EINVAL;
	}

	kfree(id_buffer);
	return len;
}

static int __init misc_init(void)
{
	return misc_register(&miscdev);
}

static void __exit misc_exit(void)
{
	misc_deregister(&miscdev);
}

module_init(misc_init);
module_exit(misc_exit);
