#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");

static const char id[] = "a02f63f544ea";
static struct dentry *subdir;
static int foo_len = 0;
static char foo_data[PAGE_SIZE];
static struct mutex foo_mutex;

static ssize_t id_read(struct file *filp,
		char *buffer,
		size_t len,
		loff_t *offset);
static ssize_t id_write(struct file *filp,
		const char *buffer,
		size_t len,
		loff_t *offset);

static ssize_t foo_read(struct file *filp,
		char *buffer,
		size_t len,
		loff_t *offset);
static ssize_t foo_write(struct file *filp,
		const char *buffer,
		size_t len,
		loff_t *offset);

const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write
};

const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write
};

static ssize_t id_read(struct file *filp,
		char *buffer,
		size_t len,
		loff_t *offset)
{
	return simple_read_from_buffer(buffer, len, offset, id, sizeof(id));
}

static ssize_t id_write(struct file *filp,
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

static ssize_t foo_read(struct file *filp,
		char *buffer,
		size_t len,
		loff_t *offset)
{
	int ret = mutex_lock_interruptible(&foo_mutex);
	if (ret)
		return ret;

	ret = simple_read_from_buffer(buffer, len, offset, foo_data, foo_len);

	mutex_unlock(&foo_mutex);
	return ret;
}

static ssize_t foo_write(struct file *filp,
		const char *buffer,
		size_t len,
		loff_t *offset)
{
	ssize_t ret = mutex_lock_interruptible(&foo_mutex);
	if (ret)
		return ret;

	len = len > PAGE_SIZE ? PAGE_SIZE : len;

	if ((ret = simple_write_to_buffer(foo_data,
				PAGE_SIZE, offset, buffer, len)) < 0) {
		return ret;
	}

	foo_len = ret;

	mutex_unlock(&foo_mutex);
	return ret;
}

static int __init dbg_init(void)
{
	struct dentry *file, *file1, *file2;

	subdir = debugfs_create_dir("eudyptula", NULL);
	if (IS_ERR(subdir))
		return PTR_ERR(subdir);
	if (!subdir)
		return -ENOENT;

	file = debugfs_create_file("id", 0666, subdir, NULL, &id_fops);
	if (!file) {
		debugfs_remove_recursive(subdir);
		return -ENOENT;
	}

	file1 = debugfs_create_u64("jiffies", 0444, subdir, (u64 *)&jiffies);
	if (!file1) {
		debugfs_remove_recursive(subdir);
		return -ENOENT;
	}

	mutex_init(&foo_mutex);
	file2 = debugfs_create_file("foo", 0644, subdir, NULL, &foo_fops);
	if (!file2) {
		debugfs_remove_recursive(subdir);
		return -ENOENT;
	}

	return 0;
}

static void __exit dbg_exit(void)
{
	debugfs_remove_recursive(subdir);
}

module_init(dbg_init);
module_exit(dbg_exit);
